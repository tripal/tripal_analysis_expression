/**
 * This is the default function that is called when the page is loaded.
 * It is also called when the reset link is clicked.
 */
(function ($) {
    $(window).on('resize', function () {
        expNormal();
    });
})(jQuery);

function expNormal() {
    heatMapTotal = '';
    heatMapTotal = JSON.parse(JSON.stringify(heatMapRaw));
    //Get samples associated with selected analysis
    selectedAnalysis = d3.select('#analyses').property('value');
    heatMap = heatMapTotal[selectedAnalysis].biomaterials;
    buildPropertySelect();
    d3.selectAll('chart').remove();
    expRewrite();
}

function expRewrite() {
    currentSorting = jQuery('#propertySortMenu').find(':selected').text();
    currentColor = jQuery('#propertyColorMenu').find(':selected').text();

    maxHeat = d3.max(heatMap, function (d) {
        return Number(d.intensity);
    });
    minHeat = d3.min(heatMap, function (d) {
        return Number(d.intensity);
    });

    //define color scale based on selected
    if (currentColor != 'Expression value') {
        var colorDomain = buildPropertyValuesDomain('color');
        var color = d3.scale.ordinal()
            .domain(colorDomain)
            .range(['#ca0020', '#f4a582', '#d5d5d5', '#92c5de', '#0571b0']);
        //TODO: CALCULATE BASED ON NUMBER OF PROPERTIES INSTEAD


    } else {
        colorDomain = [0, maxHeat / 2, maxHeat];
        var color = d3.scale.linear()
            .domain(colorDomain)
            .range(['blue', 'gray', 'red']);
    }

    var width = d3.select('figure').node().getBoundingClientRect().width;

    var height = 500;
    // var margin = 20;
    var margin = {top: 50, bottom: 100, horizontal: 20};

    var svg = d3.select('figure')
        .append('chart')
        .append('svg')
        .attr('width', width)
        .attr('height', height)
        .style('overflow-x', 'auto')
        .append('g');

    propertyValueList = buildPropertyValuesDomain();

    var x0 = d3.scale.ordinal()
        .rangeRoundBands([margin.horizontal, width]);

    var y = d3.scale.linear()
        .range([height, (margin.top + margin.bottom)]);//reverse because 0 is the top.

    var yAxis = d3.svg.axis()
        .scale(y)
        .orient('left')
        .ticks(2);

    var nested = d3.nest()
        .key(function (d) {
            if (!d.properties[currentSorting]) {
                return 'Not set';
            }
            return d.properties[currentSorting];
        }).entries(heatMap);

    //set the domains based on the nested data
    x0.domain(nested.map(function (d) {
        return d.key;
    }));
    y.domain([0, maxHeat]);

    svg.append('g')
        .attr('class', 'y-axis')
        .attr('transform', 'translate(' + (2.5 * margin.horizontal) + ', -' + (margin.bottom) + ')')
        .style('font-size', '12px')
        .style('font-weight', 'normal')
        .call(yAxis)
        .select('.domain');

    var dragging = {};

    var propertyGroups = svg.selectAll('.propertyGroups')
        .data(nested)
        .enter()
        .append('g')
        .attr('transform', function (d) {
            return 'translate(' + translationXOffset(d, x0) + ',0)';
        });

    var text = propertyGroups.append('text')
        .attr('class', 'label')
        .style('font-size', '10px')
        .style('font-weight', 'normal')
        .html(function (d) {
            //TODO:  PUT SPLIT KEY INTO A TEXTSPAN AS HERE http://bl.ocks.org/enjalot/1829187
            var label = d.key;
            //  characterLimit = 5
            //  if (label.length > characterLimit){
            //      splitString = label.match(new RegExp('.{1,' + characterLimit + '}', 'g'))
            //     label = splitString.join("<br>")
            //     // label = splitString
            // }
            return label;
        })
        .style('text-anchor', 'bottom');

    text.attr('transform', function (d) {
        return ' translate( 0,' + (height - margin.bottom + 10) + ' ),rotate(20)';
    });


    propertyGroups.call(d3.behavior.drag()
        .origin(function (d) {  //define the start drag as the middle of the group
            //this should match the transformation used when assigning the group
            return {x: translationXOffset(d, x0)};
        })
        .on('dragstart', function (d) {
            //track the position of the selected group in the dragging object
            dragging[d.key] = translationXOffset(d, x0);
            sel = d3.select(this);
            sel.moveToFront();
        })
        .on('drag', function (d) {//track current drag location
            dragging[d.key] = d3.event.x;

            nested.sort(function (a, b) {
                return position(a) - position(b);
            });
            x0.domain(nested.map(function (d) {//reset the x0 domain
                return d.key;
            }));
            propertyGroups.attr('transform', function (d) {
                return 'translate(' + position(d) + ', 0)';
            });

        })
        .on('dragend', function (d) {
            delete dragging[d.key];
            transition(d3.select(this)).attr('transform', function (d) {
                return 'translate(' + translationXOffset(d, x0) + ',0)';
            });
            propertyGroups.selectAll()
                .attr('transform', function (d) {
                    return 'translate(' + translationXOffset(d, x0) + ',0)';
                });
        })
    );

    var bars = propertyGroups.selectAll('.bar')
        .data(function (d) {
            return d.values;
        })//nest() creates key:values.  for us, key is the property value, and values are the full biomat object
        .enter().append('rect')
        .style('fill', function (d) { // fill depends on if user is doing expression based or property based
            if (currentColor == 'Expression value') {
                return color(d.intensity);
            }
            else {
                if (!d.properties[currentColor]) {
                    propertyName = 'Not set';
                } else {
                    propertyName = d.properties[currentColor];
                }
                return color(propertyName);
            }

        })
        .attr('y', function (d) {
            return y(d.intensity);
        })
        .attr('x', function (d, i) {
            numberBiosamples = Object.keys(d).length;

            return 10 * i - (1 / numberBiosamples * 10);
        })
        .attr('width', 9)
        .attr('height', function (d) {
            return y(0) - y(d.intensity);
        })
        .attr('transform', 'translate(0,' + (-margin.bottom) + ')');


    //define methods for dragging
    d3.selection.prototype.moveToFront = function () {
        return this.each(function () {
            this.parentNode.appendChild(this);
        });
    };

    function position(property) {
        var v = dragging[property.key];
        //v will be null if we arent dragging it: in that case, get its position
        return v == null ? translationXOffset(property, x0) : v;
    }

    function transition(g) {
        return g.transition().duration(500);
    }

    //Add the tool tip

    var divTooltip = d3.select('body').append('div')
        .attr('class', 'toolTip')
        .style('position', 'absolute')
        .style('max-width', '250px')
        .style('padding', '10px')
        .style('font-size', '12px')
        .style('font-family', 'Helvetica, Roboto, sans-serif')
        .style('background', 'rgba(255, 255, 255, .9)')
        .style('border', '1px solid rgba(0,0,0,.3)')
        .style('border-radius', '5px')
        .style('pointer-events', 'none')
        .style('display', 'none')
        .style('opacity', 0)
        .style('transition', 'opacity .25s linear')
        .style('z-index', 999999);
    bars.on('mouseover', function (d) {
        propTable = buildPropertyTooltipTable(d);
        divTooltip.transition()
            .duration(200)
            .style('opacity', 1)
            .style('display', 'block');
        divTooltip.html(
            '<strong>Biosample:</strong> ' + d.name + '<br/>' +
            '<strong>Expression: </strong>' + d.intensity + ' ' + d.units + '<br/>' +
            '<strong>Description: </strong><br/>' + d.description + '<br/>'
            + propTable)
            .style('left', (jQuery(this).offset().left - 260) + 'px')
            .style('top', (jQuery(this).offset().top - (y(d.intensity)) + 'px'));//(d3.event.pageY))// - (height + margin)) + "px");
    })

        .on('mouseout', function (d) {
            divTooltip.transition()
                .duration(500)
                .style('opacity', 0)
                .style('display', 'none');
        });

    buildLegend(color, width, margin);

    var title = 'Expression by ' + currentSorting;

    svg.append('text')
        .attr('x', (width / 2 - margin.horizontal))
        .attr('y', 0 + (margin.top / 2))
        .attr('text-anchor', 'middle')
        .style('font-size', '16px')
        .text(title);


}

/**This function determines the position of a nested group by returning the value from the scale and adding half the range band.
 *
 * @param d
 * @param scale
 * @returns {string}
 */
function translationXOffset(d, scale) {

    return (scale(d.key) + scale.rangeBand() / 2);
}

/**Builds the legend.
 *
 * @param colorScale
 * @param width
 * @param margin
 */
function buildLegend(colorScale, width, margin) {
    currentColor = jQuery('#propertyColorMenu').find(':selected').text();
    d3.select('chart').selectAll('legend').remove();

    if (currentColor != 'Expression value') {
        var legend = d3.select('chart')
            .select('g')
            .append('g')
            .attr('class', 'legend')
            .attr('transform', 'translate(' + (width - 10 * margin.horizontal) + ', 10)')
            .selectAll('.legendItem')
            .data(colorScale.domain())
            .enter()
            .append('g')
            .attr('class', 'legendItem')
            .attr('transform', function (d, i) {
                {
                    return 'translate(0,' + (i * 10) + ' )';
                }
            });
        legend.append('rect')
            .attr('x', 00)
            .attr('y', 10)
            .attr('width', 20)
            .attr('height', 10)
            .style('fill', function (d, i) {
                return colorScale(d);
            });
        legend.append('text')
            .attr('x', 30)
            .attr('y', 20)
            .text(function (d, i) {
                return d;
            })
            .attr('class', 'textselected')
            .style('text-anchor', 'start')
            .style('font-size', 12);

        //Now add title
        d3.select('chart').select('.legend')
            .append('text')
            .text(currentColor)
            .style('font-size', 12);

    }
    else {
        var legend = d3.select('chart')
            .select('g')
            .append('g')
            .attr('class', 'legend')
            .attr('transform', 'translate(' + (width - 10 * margin.horizontal) + ', 10)');
        //we need the min/max value and the color range.  Let's also round.
        var minHeatValue = Math.round(colorScale.domain()[0]);
        var midHeatValue = Math.round(colorScale.domain()[1]);
        var maxHeatValue = Math.round(colorScale.domain()[2]);
        var minHeatColor = colorScale.range()[0];
        var midHeatColor = colorScale.range()[1];
        var maxHeatColor = colorScale.range()[2];
        //TODO: Refactor all of this to map the domain values and loop through
        legend.append('text')
            .attr('x', 0)
            .attr('y', 0)
            .text('Expression value')
            .style('font-size', 12);
        legend.append('rect')
            .attr('x', 20)
            .attr('y', 10)
            .attr('width', 20)
            .attr('height', 10)
            .style('fill', minHeatColor);
        legend.append('text')
            .attr('x', 50)
            .attr('y', 20)
            .text(minHeatValue)
            .attr('class', 'text')
            .style('text-anchor', 'start')
            .style('font-size', 12);
        legend.append('rect')
            .attr('x', 20)
            .attr('y', 20)
            .attr('width', 20)
            .attr('height', 10)
            .style('fill', midHeatColor);
        legend.append('text')
            .attr('x', 50)
            .attr('y', 30)
            .text(midHeatValue)
            .attr('class', 'text')
            .style('text-anchor', 'start')
            .style('font-size', 12);
        legend.append('rect')
            .attr('x', 20)
            .attr('y', 30)
            .attr('width', 20)
            .attr('height', 10)
            .style('fill', maxHeatColor);
        legend.append('text')
            .attr('x', 50)
            .attr('y', 40)
            .text(maxHeatValue)
            .attr('class', 'text')
            .style('text-anchor', 'start')
            .style('font-size', 12);

    }
    d3.select('.legend').call(d3.behavior.drag()    //Add drag behavior to legend
        .on('drag', function () {
                //Update the current position
                var x = d3.event.x;
                var y = d3.event.y;
                d3.select(this).attr('transform', 'translate(' + x + ',' + y + ')');
            }
        )
    );
}


/**
 * Append the Property selectors.  Sort is for what property to group by, color is for what property to color by.
 */
function buildPropertySelect() {

    //remove the old selectors and store values
    var previousValueSort = jQuery('#propertySortMenu').find(':selected').text();
    d3.selectAll('#propertySortDiv').selectAll('select').remove();
    var previousValueColor = jQuery('#propertyColorMenu').find(':selected').text();
    d3.selectAll('#propertyColorDiv').selectAll('select').remove();

    //build list of properties for this analysis
    var selectorSort = d3.select('#propertySortDiv').append('select').attr('id', 'propertySortMenu');
    var selectorColor = d3.select('#propertyColorDiv').append('select').attr('id', 'propertyColorMenu');

    //first add "expression value" as default for color

    selectorColor.append('option')
        .attr('value', 'Expression value')
        .text('Expression value');

    heatMap.map(function (biomaterial) {
        Object.keys(biomaterial.properties).map(function (property_key) {
            //determine if this property is already in our selector
            var exists = jQuery('#propertySortDiv option')
                .filter(function (i, o) {
                    return o.value === property_key;
                })
                .length > 0;

            if (!exists) { //append it to both selector lists
                selectorSort.append('option')
                    .attr('value', function () {
                        return property_key;
                    })
                    .text(function () {
                        return property_key;
                    });
                selectorColor.append('option')
                    .attr('value', function () {
                        return property_key;
                    })
                    .text(function () {
                        return property_key;
                    });
            }

        });
    });

    jQuery('#propertyColorMenu').val('Expression value');

    if (previousValueSort) {
        jQuery('#propertySortMenu').val(previousValueSort);
    }
    if (previousValueColor) {
        jQuery('#propertyColorMenu').val(previousValueColor);
    }
    //if the selector changes, rebuild the figure
    jQuery('#propertySortMenu').change(function () {
        d3.selectAll('chart').remove();
        expRewrite();
    });
    //if the selector changes, rebuild the figure
    jQuery('#propertyColorMenu').change(function () {
        d3.selectAll('chart').remove();
        expRewrite();
    });
}

/**
 * Build an array with lists of biomaterials sorted by value.
 * if the color param is passed in as "color" it will build the domain from the color property instead of the sort property.
 * @returns {Array}
 */

function buildPropertyValuesDomain(color) {
    if (color == 'color') {
        currentSortingProperty = jQuery('#propertyColorMenu').find(':selected').text();
    } else {
        currentSortingProperty = jQuery('#propertySortMenu').find(':selected').text();
    }
    list = [];
    heatMap.map(function (biomaterial) {
        name = biomaterial.name;
        propertyValue = biomaterial.properties[currentSortingProperty];
        if (!propertyValue) {
            propertyValue = 'Not set';
        }
        list.push(propertyValue);
    });
    Array.prototype.unique = function () {
        var arr = this;
        return jQuery.grep(arr, function (v, i) {
            return jQuery.inArray(v, arr) === i;
        });
    };
    return list.unique();
}


/**
 * This function will remove biomaterials that have a value of 0.
 */
function nonZero() {
    heatMap = heatMap.filter(function (d) {
        return d.intensity > 0;
    });
    d3.selectAll('chart').remove();
    expRewrite();
}


/**
 * Creates the table for the tooltip
 * @param d
 * @returns {string}
 */
function buildPropertyTooltipTable(d) {
    table = '';
    if (Object.keys(d.properties).length > 0) {
        table += '</br><table class="expression-tooltip-table"> <tr>' +
            '    <th>Property Name</th>' +
            '    <th>Property Value</th> ' +
            '  </tr>';
        Object.keys(d.properties).map(function (propertyKey) {
            propertyValue = d.properties[propertyKey];
            table += '<tr><td>' + propertyKey + '</td><td>' + propertyValue + '</td> </tr>';
        });
        table += '</table>';
    }
    return (table);
}