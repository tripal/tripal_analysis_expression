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

    //remove the old selector and store its value
    previousValue = jQuery("#propertyMenu").find(":selected").text()
    d3.selectAll('#propertyDiv').select("select").remove()

    //build list of properties for this analysis
    var selector = d3.select("#propertyDiv").append("select").attr("id", "propertyMenu")

    heatMap.map(function (biomaterial) {
        Object.keys(biomaterial.properties).map(function (property_key) {
          //determine if this property is already in our selector
            var exists = $("#propertyDiv option")
                .filter(function (i, o) {
                    return o.value === property_key;
                })
                .length > 0;

            if (!exists) {
                selector.append("option")
                    .attr("value", function () {
                        return property_key;
                    })
                    .text(function () {
                        return property_key;
                    })
            }

        })
    })

    if (previousValue) {
        jQuery("#propertyMenu").val(previousValue)
    }
    //if the selector changes, rebuild the figure
    jQuery("#propertyMenu").change(function () {
        d3.selectAll('chart').remove();
        expRewrite();
    })
}


/**
 * This function is called to change the graph type to chart.
 */
function expChart() {
    if (col == 'column') {
        col = 'tile';
    }
    else if (col == 'tile') {
        col = 'column';
    }
    d3.selectAll('expfeaturedom').remove();
    d3.selectAll('expkeydom').remove();
    exp();
}

/**
 * Build an array with lists of biomaterials sorted by value.
 * .nest() would do this but not avialable in d3.v3
 * @returns {Array}
 */
function sortDataByProperty() {
    currentSortingProperty = jQuery("#propertyMenu").find(":selected").text()
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
function buildPropertyValuesDomain() {
    currentSortingProperty = jQuery("#propertyMenu").find(":selected").text()
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
 * This will arrange the biomaterials by value from greatest to least.
 */
function expSortDown() {
    heatMap.sort(function (a, b) {
        return Number(b.intensity) - Number(a.intensity);
    });
    d3.selectAll('expkeydom').remove();
    expRewrite();
}

/**
 * This will arrange the biomaterials by value from least to greatest.
 */
function expSortUp() {
    heatMap.sort(function (a, b) {
        return Number(a.intensity) - Number(b.intensity);
    });
    d3.selectAll('expfeaturedom').remove();
    d3.selectAll('expkeydom').remove();
    expRewrite();
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
    currentSorting = jQuery("#propertyMenu").find(":selected").text()
    structuredMap = sortDataByProperty()
    var width = d3.select('figure').node().getBoundingClientRect().width;
    var height = 200;
    var margin = {top: 5, right: 5, bottom: 5, left: 5};


    var svg = d3.select('figure').append('chart').append('svg')
        .attr('width', width)
        .attr('height', height)
        .append('g')
        // .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');
    maxHeat = d3.max(heatMap, function (d) {
        return Number(d.intensity);
    });
    minHeat = d3.min(heatMap, function (d) {
        return Number(d.intensity);
    });


    propertyValueList = buildPropertyValuesDomain()

    var x0 = d3.scale.ordinal()
        .rangeRoundBands([0, width], .05)
    // var x1 = d3.scale.ordinal()
    //     .rangeRoundBands([0, x0.rangeBand()], .1)
    var y = d3.scale.linear()
        .range([height, 0])//reverse because 0 is the top


    var colorRange = d3.scale.category20();
    var color = d3.scale.ordinal()
        .range(colorRange.range());

    var xAxis = d3.svg.axis()
        .scale(x0)
        .orient("bottom");

    var yAxis = d3.svg.axis()
        .scale(y)
        .orient("left")
    // .tickFormat(d3.format(".2s"));

    var divTooltip = d3.select("figure").append('chart').append("div").attr("class", "toolTip");


    var nested = d3.nest()
        .key(function (d) {
            if (!d.properties[currentSorting]) {
                return "Not set"
            }
            return d.properties[currentSorting];
        }).entries(heatMap)

    var groups = svg.selectAll(null)
        .data(structuredMap)
        .enter()
        .append("g")
        .attr("height", height + margin.top + margin.bottom)
        .attr("width", width + margin.left + margin.right)
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")")
    ;

    //set the domains based on the nested data
    x0.domain(nested.map(function (d) {
        return d.key
    }))
    y.domain([0, maxHeat])
//append the x and y axes
    //TODO: FIX THESE TRANSFORMATIONS
    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height  + ")")
        .call(xAxis)

    svg.append("g")
        .attr("class", "y axis")
        .attr("transform", "translate(0," + ( margin.bottom) + ")")
        .call(yAxis)
        .append("text")
        .attr("transform", "rotate(-90)")
        .attr("dy", ".71em")
        .style("text-anchor", "end")
        .text("Expression units");

    var propertyGroups = svg.selectAll(".propertyGroups")
        .data(nested)
        .enter()
        .append("g")
        .attr("transform", function (d) {
            return "translate(0," + x0(d.key) + ")"
        }) //position each group where it is found in x0

    var bars = propertyGroups.selectAll(".bar")
        .data(function (d) {
            return d.values
        })//nest() creates key:values.  for us, key is the property value, and values are the full biomat object
        .enter().append("rect")
    bars.style("fill", function (d, i) {
        return color(i - 2)
    })
        .attr("y", function(d) {
            return y(d.intensity)
        })
        .attr("x", function (d, i) {
            if (i % 2 == 0) { //if this is even or odd we want to shift it along the rangeband a little bit
                return (x0.rangeBand() / 2 ) + (i / 2 + 0.5) * 10
            } else {
                return (x0.rangeBand() / 2 ) - (i / 2) * 10
            }

        })
        .attr("width", 5)
        .attr("height", function (d) {
            return height - y(d.intensity)
        })

    // var labels = propertyGroups.selectAll(".label")
    //     .data(function (d) {
    //         return d.values
    //     })
    //     .enter().append("text");
    //
    // labels.text(function (d) {
    //     return (d.intensity);
    // })
    //     .attr("text-anchor", "start")
    //     .attr("y", function (d) {
    //         return y(d.intensity) + 20
    //     })
    //     .attr("x", function (d, i) {
    //         if (i % 2 == 0) {
    //             return (x0.rangeBand() / 2 - 2) + (i / 2 + 0.5) * 10
    //         }
    //         else {
    //             return (x0.rangeBand() / 2 - 2) - (i / 2) * 10
    //         }
    //     })
    //     .attr("class", "axis");


}