//lets follow this tutorial to create a simple chart
///https://www.pshrmn.com/tutorials/d3/bar-charts/

//draggability:
//http://bl.ocks.org/AlessandraSozzi/e588d03fb7a7bb07c250

//grouping:
//https://bl.ocks.org/mbostock/3887051

//This grouping uses v3 i think
//http://bl.ocks.org/juan-cb/ac731adaeadd3e855d26


//n https://jsfiddle.net/hb13oe4v/

/**
 * This is the default function that is called when the page is loaded.
 * It is also called when the reset link is clicked. It will order biomaterials
 * using alphanumeric ordering.
 */
function expNormal() {
    heatMapTotal = '';
    heatMapTotal = JSON.parse(JSON.stringify(heatMapRaw));
    //Get samples associated with selected analysis
    selectedAnalysis = d3.select('#analyses').property("value");
    heatMap = heatMapTotal[selectedAnalysis].biomaterials;
    buildPropertySelect();
    d3.selectAll('chart').remove();
    expRewrite();
}

/**
 * Append the Property selector
 */
function buildPropertySelect() {

    //remove the old selectors and store values
    previousValueSort = jQuery("#propertySortMenu").find(":selected").text()
    d3.selectAll('#propertySortDiv').selectAll("select").remove()
    previousValueColor = jQuery("#propertyColorMenu").find(":selected").text()
    d3.selectAll('#propertyColorDiv').selectAll("select").remove()

    //build list of properties for this analysis
    var selectorSort = d3.select("#propertySortDiv").append("select").attr("id", "propertySortMenu")
    var selectorColor = d3.select("#propertyColorDiv").append("select").attr("id", "propertyColorMenu")

    //first add "expression value" as default for color

    selectorColor.append("option")
        .attr("value", "expressionValue")
        .text("Expression value")

    heatMap.map(function (biomaterial) {
        Object.keys(biomaterial.properties).map(function (property_key) {
            //determine if this property is already in our selector
            var exists = $("#propertySortDiv option")
                .filter(function (i, o) {
                    return o.value === property_key;
                })
                .length > 0;

            if (!exists) {
                selectorSort.append("option")
                    .attr("value", function () {
                        return property_key;
                    })
                    .text(function () {
                        return property_key;
                    })
                selectorColor.append("option")
                    .attr("value", function () {
                        return property_key;
                    })
                    .text(function () {
                        return property_key;
                    })
            }

        })
    })

    if (previousValueSort) {
        jQuery("#propertySortMenu").val(previousValueSort)
    }
    if (previousValueColor) {
        jQuery("#propertyColorMenu").val(previousValueColor)
    } else {
        jQuery("#propertyColorMenu").val("expressionValue")
    }
    //if the selector changes, rebuild the figure
    jQuery("#propertySortMenu").change(function () {
        d3.selectAll('chart').remove();
        expRewrite();
    })
    //if the selector changes, rebuild the figure
    jQuery("#propertyColorMenu").change(function () {
        d3.selectAll('chart').remove();
        expRewrite();
    })
}

/**
 * Build an array with lists of biomaterials sorted by value.
 * @returns {Array}
 */

//TODO:  I DONT THINK WE USE THIS?
function sortDataByProperty() {
    currentSortingProperty = jQuery("#propertySortMenu").find(":selected").text()
    list = []
    heatMap.map(function (biomaterial) {
        name = biomaterial.name
        propertyValue = biomaterial.properties[currentSortingProperty]
        if (!propertyValue) {
            propertyValue = "Not set"
        }
        signal = biomaterial.intensity
        allProperties = biomaterial.properties
        entry = {name: name, propertyValue: propertyValue, signal: signal, allProperties: allProperties}
        if (!list[propertyValue]) {
            list[propertyValue] = [entry]
        }
        else {
            existingList = list[propertyValue]
            existingList.push(entry)
            list[propertyValue] = existingList
        }

    })

    return list
}

/**
 * Build an array with lists of biomaterials sorted by value
 * @returns {Array}
 */
//TODO:  I  THINK WE USE THIS TO SET DOMAIN BUT DOUBLE CHECK

function buildPropertyValuesDomain() {
    // if (color) {
    //     currentSortingProperty = jQuery("#propertyColorMenu").find(":selected").text()
    // }
    currentSortingProperty = jQuery("#propertySortMenu").find(":selected").text()
    list = []
    heatMap.map(function (biomaterial) {
        name = biomaterial.name
        propertyValue = biomaterial.properties[currentSortingProperty]
        if (!propertyValue) {
            propertyValue = "Not set"
        }
        list.push(propertyValue)
    })
    Array.prototype.unique = function () {
        var arr = this;
        return $.grep(arr, function (v, i) {
            return $.inArray(v, arr) === i;
        });
    }
    return list.unique()
}


/**
 * This function will remove biomaterials that have a value of 0.
 */
function nonZero() {
    heatMap = heatMap.filter(function (d) {
        return d.intensity > 0;
    });
    d3.selectAll('expfeaturedom').remove();
    d3.selectAll('expkeydom').remove();
    expRewrite();
}

function expRewrite() {
    currentSorting = jQuery("#propertySortMenu").find(":selected").text()

    structuredMap = sortDataByProperty()

    var width = d3.select('figure').node().getBoundingClientRect().width;

    var height = 300;
    var margin = 20;


    var svg = d3.select('figure')
        .append('chart')
        .append('svg')
        .attr('width', width)
        .attr('height', height)
        .append('g')

    maxHeat = d3.max(heatMap, function (d) {
        return Number(d.intensity);
    });
    minHeat = d3.min(heatMap, function (d) {
        return Number(d.intensity);
    });


    propertyValueList = buildPropertyValuesDomain()

    var x0 = d3.scale.ordinal()
        .rangeRoundBands([margin, width])

    // var x1 = d3.scale.ordinal()
    //     .rangeRoundBands([0, x0.rangeBand()], .1)
    var y = d3.scale.linear()
        .range([height, (0 + margin)])//reverse because 0 is the top

    var colorRange = d3.scale.category20();
    var color = d3.scale.ordinal()
        .range(colorRange.range());

    var xAxis = d3.svg.axis()
        .scale(x0)
        .orient("bottom");

    var yAxis = d3.svg.axis()
        .scale(y)
        .orient("left")
        .ticks(2)

    // var divTooltip = d3.select("figure").append('chart').append("div").attr("class", "toolTip");


    var nested = d3.nest()
        .key(function (d) {
            if (!d.properties[currentSorting]) {
                return "Not set"
            }
            return d.properties[currentSorting];
        }).entries(heatMap)


    //set the domains based on the nested data
    x0.domain(nested.map(function (d) {
        return d.key
    }))


    y.domain([0, maxHeat])


//append the x and y axes
    //   TODO: FIX THE Y-scale TRANSFORMATIONS
    // svg.append("g")
    //     .attr("class", "x-axis")
    //     .attr("transform", "translate(0," + (height - margin) + ")")
    //     .call(xAxis)
    //     .selectAll("text")
    //     .style("font-size","12px")
    //     .style("font-weight","normal")

    svg.append("g")
        .attr("class", "y-axis")
        .attr("transform", "translate(" + (2.5 * margin) + ", -" + margin + ")")
        .call(yAxis)

    var dragging = {}

    var propertyGroups = svg.selectAll(".propertyGroups")
        .data(nested)
        .enter()
        .append("g")


        propertyGroups.append("text")
            .attr("class", "label")
            .attr("x", function(d){
                    return (x0(d.key) + x0.rangeBand() / 2)
            } )
            .attr("y", height - margin/3)
            .attr("transform", "translate(0, 0)")
        .style("font-size", "12px")
        .style("fonx-weight", "normal")
            .text(function(d){
                return d.key
            } )
            .style("text-anchor", "middle")

            // .attr("x", function (d, i) {
            //     var propertyName
            //
            //     if (!d.properties[currentSorting]) {
            //         propertyName = "Not set"
            //     } else {
            //         propertyName = d.properties[currentSorting]
            //     }
            //     return (x0(propertyName) + x0.rangeBand() / 2 + i * 10  )
            // })
            // .attr("width", 5)
            // .attr("height", function (d) {
            //     return y(0) - y(d.intensity)
            // })
            // .attr("transform", "translate(0," + ( -margin) + ")")

        propertyGroups.call(d3.behavior.drag()
            .origin(function (d) {  //define the start drag as the middle of the group
                return {
                    x: x0(d.key) - margin
                };
            })
            .on("dragstart", function (d) {
                //track the position of the selected group in the dragging object
                dragging[d.key] = x0(d.key);
                sel = d3.select(this);
                sel.moveToFront();
            })
            .on("drag", function (d) {//track current drag location
                //dragging[d.key] = Math.min(-200, Math.max(0, d3.event.x));
                dragging[d.key] =  d3.event.x;

                nested.sort(function (a, b) {
                    //compare function:  if less than 0, a comes first
                    //compare function if returns 0, leave unchanged
                    // greater than 0, b comes first
                    return position(a) - position(b);
                })
                x0.domain(nested.map(function (d) {//reset the x0 domain
                    return d.key
                }))
                propertyGroups.attr("transform", function (d) {
                    return "translate(" + position(d) + ", 0)";
                })

            })
            .on("dragend", function (d) {
                delete dragging[d.key];
                transition(d3.select(this)).attr("transform", "translate(" + (x0(d.key)- margin ) + ", 0)");

                propertyGroups.selectAll()
                    .attr("x", function(d){
                        return (x0(d.key) + x0.rangeBand() / 2)
                    } )
            })
        )

//TODO:  FIX THIS!  Right now it uses the property values domain to just get the other selector's domain
//define color scale based on selected
    currentColor = jQuery("#propertyColorMenu").find(":selected").text()
    if (currentColor) {
        var colorDomain = buildPropertyValuesDomain()
        var color = d3.scale.ordinal()
            .domain(colorDomain)
            .range(["#ca0020","#f4a582","#d5d5d5","#92c5de","#0571b0"]);
    }


    var bars = propertyGroups.selectAll(".bar")
        .data(function (d) {
            return d.values
        })//nest() creates key:values.  for us, key is the property value, and values are the full biomat object
        .enter().append("rect")
        .style("fill", function (d, i) {

            if (!d.properties[currentColor]) {
                propertyName = "Not set"
            } else {
                propertyName = d.properties[currentSorting]
            }
            return color(propertyName)

        })
        .attr("y", function (d) {
            return y(d.intensity)
        })
        .attr("x", function (d, i) {
            var propertyName

            if (!d.properties[currentSorting]) {
                propertyName = "Not set"
            } else {
                propertyName = d.properties[currentSorting]
            }
            return (x0(propertyName) + x0.rangeBand() / 2 + i * 10  )
        })
        .attr("width", 5)
        .attr("height", function (d) {
            return y(0) - y(d.intensity)
        })
        .attr("transform", "translate(0," + ( -margin) + ")")


    //define methods for dragging
    d3.selection.prototype.moveToFront = function () {
        return this.each(function () {
            this.parentNode.appendChild(this);
        });
    };

    function position(property) {
        var v = dragging[property.key];
        //v will be null if we arent dragging it: in that case, get its position
        return v == null ? x0(property.key) : v;
    }

    function transition(g) {
        return g.transition().duration(500);
    }

}

