(function ($) {

  Drupal.behaviors.expression = {
    attach: function (context, settings) {
      // Define variables
      this.heatMapRaw       = JSON.parse(settings.heatMapRaw)
      this.selectedAnalysis = settings.selectedAnalysis
      this.feature_id       = settings.feature_id
      this.heatMapTotal     = ''
      this.heatMap          = []
      this.currentSorting   = $('#propertySortMenu').find(':selected').text()
      this.currentColor     = $('#propertyColorMenu').find(':selected').text()
      this.downloadMessage  = $('<span />')
      this.downloadLink     = $('#expressionDownloadLink')
      //spacing variables
      this.margin           = {top: 50, bottom: 100, horizontal: 20}
      this.height           = 500
      this.barwidth         = 12
      this.scaleSize        = this.barwidth * 4.5 //this is a thumbnail
      // estimate, im not sure of a
      // better way to do it
      this.barSpacing = this.barwidth / 2


      // Activate plot
      this.expNormal()
      this.attachEventListeners()
    },

    /**
     * Attach events to all of our elements here.
     */
    attachEventListeners: function () {
      $('#show-non-zero-only').on('click', this.nonZero.bind(this))
      $('#reset-expression-plot').on('click', this.expNormal.bind(this))
      $('#analyses-dropdown').on('change', this.analysisChanged.bind(this))
      $(window).on('resize', this.expNormal.bind(this))
      $(document).on('tripal_ds_pane_expanded', this.expNormal.bind(this))

      // If the selector changes, rebuild the figure
      $(document).on('change', '#propertySortMenu', function (e) {
        this._handleSortMenuChange(e)
      }.bind(this))
      setTimeout(function () {
        this._handleSortMenuChange()
      }.bind(this), 500)

      // If the selector changes, rebuild the figure
      $(document).on('change', '#propertyColorMenu', function (e) {
        this.currentColor = e.target.value
        d3.selectAll('chart').remove()
        this.drawPlot()
      }.bind(this))

      var _that = this
      this.downloadLink.after(this.downloadMessage)
      this.downloadLink.on('click', function (e) {
        _that.initiateDownload.call(this, e, _that)
      }) // Do not bind this here
    },

    /**
     * Handle sort menu changes.
     *
     * @param e
     * @private
     */
    _handleSortMenuChange: function (e) {
      this.currentSorting = $('#propertySortMenu').val()
      d3.selectAll('chart').remove()
      this.drawPlot()
    },

    /**
     * Create an iframe to start the download.
     *
     * @param {Object} e event
     * @param {Object} object this
     */
    initiateDownload: function (e, object) {
      return
      e.preventDefault()
      var src    = $(this).attr('href')
      var iframe = $('<iframe />', {
        src   : src,
        width : 1,
        height: 1,
      })
      $('body').append(iframe)
      object.downloadMessage.html(' Generating file. Download will start automatically...')
    },

    /**
     * updates the download link when an analysis has been selected.
     */
    analysisChanged: function (e) {
      this.selectedAnalysis = e.target.value
      this.expNormal()

      // Change the link address
      var link = '/tripal/analysis-expression/download?feature_ids=' + this.feature_id + '&analysis_id=' + this.selectedAnalysis
      $('#expressionDownloadLink').attr('href', link)
    },

    /**
     * This function will remove biomaterials that have a value of 0.
     */
    nonZero: function () {
      this.heatMap = this.heatMap.filter(function (d) {
        return d.intensity > 0
      })
      d3.selectAll('chart').remove()
      this.drawPlot()
    },

    /**
     * Normalize
     */
    expNormal: function () {
      this.heatMapTotal     = this.heatMapRaw
      // Get samples associated with selected analysis
      this.selectedAnalysis = d3.select('#analyses-dropdown').property('value')
      this.heatMap          = this.heatMapTotal[this.selectedAnalysis].biomaterials
      this.buildPropertySelect()
      d3.selectAll('chart').remove()
      this.drawPlot()
    },

    /**
     * Append the Property selectors.  Sort is for what property to group by,
     * color is for what property to color by.
     */
    buildPropertySelect: function () {
      var $sortMenu         = $('#propertySortMenu')
      var $colorMenu        = $('#propertyColorMenu')
      // Remove the old selectors and store values
      var previousValueSort = $sortMenu.find(':selected').text()
      d3.selectAll('#propertySortDiv').selectAll('select').remove()
      var previousValueColor = $colorMenu.find(':selected').text()
      d3.selectAll('#propertyColorDiv').selectAll('select').remove()

      // Build list of properties for this analysis
      var selectorSort  = d3.select('#propertySortDiv').append('select').attr('id', 'propertySortMenu')
      var selectorColor = d3.select('#propertyColorDiv').append('select').attr('id', 'propertyColorMenu')

      // First add "expression value" as default for color
      selectorColor.append('option')
        .attr('value', 'Expression value')
        .text('Expression value')

      this.heatMap.map(function (biomaterial) {
        Object.keys(biomaterial.properties).map(function (property_key) {
          // Determine if this property is already in our selector
          var exists = $('#propertySortDiv option')
            .filter(function (i, o) {
              return o.value === property_key
            })
            .length > 0

          if (!exists) { //append it to both selector lists
            selectorSort.append('option')
              .attr('value', function () {
                return property_key
              })
              .text(function () {
                return property_key
              })
            selectorColor.append('option')
              .attr('value', function () {
                return property_key
              })
              .text(function () {
                return property_key
              })
          }

        })
      })

      $colorMenu.val('Expression value')

      if (previousValueSort) {
        $sortMenu.val(previousValueSort)
      }
      if (previousValueColor) {
        $colorMenu.val(previousValueColor)
      }
    },

    drawPlot: function () {
      var _that = this

      var maxHeat = d3.max(this.heatMap, function (d) {
        return Number(d.intensity)
      })

      var minHeat = d3.min(this.heatMap, function (d) {
        return Number(d.intensity)
      })

      // Define color scale based on selected
      var colorDomain = ''
      var color       = ''
      if (this.currentColor !== 'Expression value') {
        colorDomain = this.buildPropertyValuesDomain('color')
        color       = d3.scale.ordinal()
          .domain(colorDomain)
          .range(['#a6cee3', '#1f78b4', '#b2df8a', '#33a02c', '#fb9a99', '#e31a1c', '#fdbf6f',
            '#ff7f00', '#cab2d6', '#6a3d9a', '#ff9', '#b15928',
          ])
      }
      else {
        // TODO: do we mean min and max or both max here?
        //we mean what is written here.
        //You can have very disjointed expression values: the minimum could be
        // 10, and the max 10000000. This domain maps one blue to 0, HALF the
        // max to gray, and the max to red.  If you do 0, min, max, you'll
        // likely end up with only two colors: gray and red.  nothing will be
        // blue.
        colorDomain = [0, maxHeat / 2, maxHeat]
        color       = d3.scale.linear()
          .domain(colorDomain)
          .range(['blue', 'gray', 'red'])
      }


      var barwidth  = this.barwidth
      var margin    = this.margin
      var scaleSize = this.scaleSize //this is a thumbnail estimate, im not
      // sure of a better way to do it
      var barSpacing = this.barSpacing

      /// y values
      var height = this.height

      var y = d3.scale.linear()
        .range([height, (margin.top + margin.bottom)])//reverse because 0 is
      // the top.

      var yAxis = d3.svg.axis()
        .scale(y)
        .orient('left')
        .ticks(2)

      var nested = d3.nest()
        .key(function (d) {
          if (!d.properties[this.currentSorting]) {
            return 'Not set'
          }
          return d.properties[this.currentSorting]
        }.bind(this)).entries(this.heatMap)


      /// X values

      var groupCount   = {}
      var totalSamples = 0
      var numberKeys   = nested.length

      nested.map(function (d) {
        groupCount[d.key] = d.values.length
        totalSamples += d.values.length
      })

      var numberOfGroups = Object.keys(groupCount).length


      //figure out size of figure
      var minWidth = d3.select('figure').node().getBoundingClientRect().width

      var width           = ((totalSamples + 1) * barwidth) + (totalSamples * this.barSpacing) + (numberOfGroups * barwidth / 2) + scaleSize + (margin.horizontal * 2)
      var calculatedWidth = Math.max(minWidth, width)

      var averageStepSize = calculatedWidth / totalSamples
      //if there is only one group the domain will break.  if thats the case,
      // append an empty group
      if (numberOfGroups === 1) { //we can't have 0 groups
        var thisGroupIndex = numberOfGroups //no need to adjust
        // because starts at
        // 0
        nested[thisGroupIndex.toString()] = {
          'key'   : 'spacing_group_for_domain',
          'values': [],
        }
        numberOfGroups++
      }

      var rangeMapper   = {}
      var lengthTracker = 0 //keep track of where we are on the scale

      nested.map(function (d, i) {
        var groupSize = d.values.length * averageStepSize
        //   var location = lengthTracker + groupSize/2 //set the location to
        // the middle of its group
        var location = lengthTracker //set the location to the start of its
        // group

        rangeMapper[d.key]    = location
        lengthTracker += groupSize
        nested[i]['position'] = i
      })
      var x0 = d3.scale.linear()
        .rangeRound(nested.map(function (d) {
          return rangeMapper[d.key]
        }))

      // Set the domains based on the nested data
      x0.domain(nested.map(function (d, i) {
        return d.position
      }))

      y.domain([0, maxHeat])

//start plotting here
      d3.select('figure')
        .style('overflow', 'auto')
        .style('max-width', '100%')

      var svg = d3.select('figure')
        .append('chart')
        .append('svg')
        .attr('width', calculatedWidth)
        .attr('height', height)
        .append('g')


      svg.append('g')
        .attr('class', 'y-axis')
        .attr('transform', 'translate(' + (scaleSize) + ', -' + (margin.bottom) + ')')
        .style('font-size', '12px')
        .style('font-weight', 'normal')
        .call(yAxis)
        .select('.domain')

      var dragging = {}

      var propertyGroups = svg.selectAll('.propertyGroups')
        .data(nested)
        .enter()
        .append('g')
        .attr('transform', function (d, i) {
          if (d.key === 'spacing_group_for_domain') {
            return //skip plotting the spacing group
          }
          return 'translate(' + this.translationXOffset(i, x0) + ',0)'
        }.bind(this))

      //Add a bar along the bottom
      propertyGroups.append('g')
        .attr()
        .append('rect')
        .style('fill', 'black')
        .attr('y', height - (margin.bottom - 1))
        .attr('x', barwidth / 2)
        .attr('width', function (d) {
          var length = d.values.length * (barwidth + barSpacing) - barwidth * 1.5
          return length > barwidth ? length : 1

        })
        .attr('height', 1)


      var text = propertyGroups.append('text')
        .attr('class', 'label')
        .style('font-size', '12px')
        .style('font-weight', 'normal')
        .style('padding', '5px')
        .attr('x', 0)
        .attr('y', 0)
        .html(function (d) {
          if (d.key === 'spacing_group_for_domain') {
            return //skip plotting the spacing group
          }

          var label          = d.key
          var characterLimit = 20
          if (label.length > characterLimit) {
            var splitString = label.match(new RegExp('.{1,' + characterLimit + '}', 'g'))
            label           = splitString.map(function (item) {
              return '<tspan x="0" dy="10">' + item + '</tspan>'
            }).join(' ')
          }
          return label
        })
        .style('text-anchor', 'bottom')

      text.attr('transform', function (d) {
        return ' translate( ' + ((d.values.length / 2) * (barwidth) + (barSpacing) * ((d.values.length - 1) / 2)) + ',' + (height - margin.bottom + 10) + ' ),rotate(45)'
      })


      propertyGroups.call(d3.behavior.drag()
        .origin(function (d, i) {  // define the start drag as the middle of the group
          // this should match the transformation used when assigning the
          // group
          return {x: _that.translationXOffset(d.position, x0)}
        })
        .on('dragstart', function (d, i) {
          // track the position of the selected group in the dragging object
          dragging[d.key] = _that.translationXOffset(d.position, x0)
          var sel         = d3.select(this)
          sel.moveToFront()
        })
        .on('drag', function (d, i) {// track current drag location
          dragging[d.key] = d3.event.x

          nested.sort(function (a, b) {
            return position(a, a.position, x0) - position(b, b.position, x0)
          })


          ///rebuild the domain
          //TODO:  This is very not - DRY, copied from initial domain set.
          // should be factored out.

          var rangeMapper   = {}
          var lengthTracker = 0 //keep track of where we are on the scale

          nested.map(function (d, i) {

            var groupSize = d.values.length * averageStepSize
            //   var location = lengthTracker + groupSize/2 //set the location
            // to the middle of its group var location = lengthTracker +
            // groupSize/2 //set the location to the middle of its group
            var location = lengthTracker //set the location to the start of its
            // group

            rangeMapper[d.key]    = location
            lengthTracker += groupSize
            nested[i]['position'] = i
          })


          x0 = d3.scale.linear()
            .rangeRound(nested.map(function (d) {
              return rangeMapper[d.key]
            }))

          // Set the domains based on the nested data
          x0.domain(nested.map(function (d, i) {
            return i
          }))

          propertyGroups.attr('transform', function (d) {

            return 'translate(' + position(d, d.position, x0) + ', 0)'
          })
        })
        .on('dragend', function (d, i) {
          delete dragging[d.key]
          transition(d3.select(this)).attr('transform', function (d) {
            return 'translate(' + _that.translationXOffset(d.position, x0) + ',0)'
          })

          // propertyGroups.selectAll()
          //     .attr('transform', function (d, i ) {
          //       return 'translate(' + _that.translationXOffset(d.position,
          // x0) + ',0)'; });
        }),
      )

      //plot the actual bars!!

      var bars = propertyGroups.selectAll('.bar')
        .data(function (d) {
          return d.values
        })
        .enter().append('rect')
        .style('fill', function (d) { // fill depends on if user is doing expression based or property based
          if (this.currentColor === 'Expression value') {
            return color(d.intensity)
          }
          else {
            var propertyName = ''
            if (!d.properties[this.currentColor]) {
              propertyName = 'Not set'
            }
            else {
              propertyName = d.properties[this.currentColor]
            }
            return color(propertyName)
          }

        }.bind(this))
        .attr('y', function (d) {
          return y(d.intensity)
        })
        .attr('x', function (d, i) {
          return (i * (barwidth + barSpacing))
        })
        .attr('width', this.barwidth)
        .attr('height', function (d) {
          return y(0) - y(d.intensity)
        })
        .attr('transform', 'translate(0,' + (-margin.bottom) + ')')


      // define methods for dragging
      d3.selection.prototype.moveToFront = function () {
        return this.each(function () {
          this.parentNode.appendChild(this)
        })
      }

      function position(property, i, x0) {
        var v = dragging[property.key]
        // v will be null if we arent dragging it: in that case, get its
        // position

        //we need this properties key in the array
        return v == null ? _that.translationXOffset(i, x0) : v
      }

      function transition(g) {
        return g.transition().duration(500)
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
        .style('z-index', 999999)

      bars.on('mouseover', function (d) {
        var propTable = _that.buildPropertyTooltipTable(d)
        divTooltip.transition()
          .duration(200)
          .style('opacity', 1)
          .style('display', 'block')

        divTooltip.html(
          '<strong>Biosample:</strong> ' + d.name + '<br/>' +
          '<strong>Expression: </strong>' + d.intensity + ' ' + d.units + '<br/>' +
          '<strong>Description: </strong><br/>' + d.description + '<br/>'
          + propTable)
          .style('left', ($(this).offset().left - 260) + 'px')
          .style('top',((d3.event.pageY - $(this).height()) + 'px'))
      })
        .on('mouseout', function (d) {
          divTooltip.transition()
            .duration(500)
            .style('opacity', 0)
            .style('display', 'none')
        })

      this.buildLegend(color, calculatedWidth, margin, height)

      var title = 'Expression by ' + this.currentSorting

      svg.append('text')
        .attr('x', (calculatedWidth / 2 - margin.horizontal))
        .attr('y', margin.top / 2)
        .attr('text-anchor', 'middle')
        .style('font-size', '16px')
        .text(title)
    },

    /**
     * This function determines the position of a nested group by returning the
     * value from the scale and adding half the range band.
     *
     * @param d
     * @param scale
     * @returns {string}
     */
    translationXOffset: function (i, scale) {
      return (scale(i)) + this.margin.horizontal + this.scaleSize
    },

    /**
     * Creates the table for the tooltip
     * TODO: Use jQuery to build the table
     * @param d
     * @returns {string}
     */
    buildPropertyTooltipTable: function (d) {
      var table = ''
      if (Object.keys(d.properties).length > 0) {
        table += '</br><table class="expression-tooltip-table"> <tr>' +
          '    <th>Property Name</th>' +
          '    <th>Property Value</th> ' +
          '  </tr>'

        Object.keys(d.properties).map(function (propertyKey) {
          var propertyValue = d.properties[propertyKey]
          table += '<tr><td>' + propertyKey + '</td><td>' + propertyValue + '</td> </tr>'
        })

        table += '</table>'
      }
      return table
    },

    /**
     * Builds the legend.
     *
     * @param colorScale
     * @param width
     * @param margin
     * @param height
     */
    buildLegend: function (colorScale, width, margin, height) {
      this.currentColor = $('#propertyColorMenu').find(':selected').text()
      d3.select('chart').selectAll('legend').remove()

      var legend = {}

      if (this.currentColor !== 'Expression value') {
        legend = d3.select('chart')
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
              return 'translate(0,' + (i * 10) + ' )'
            }
          })
        legend.append('rect')
          .attr('x', 0)
          .attr('y', 10)
          .attr('width', 20)
          .attr('height', 10)
          .style('fill', function (d, i) {
            return colorScale(d)
          })
        legend.append('text')
          .attr('x', 30)
          .attr('y', 20)
          .text(function (d, i) {
            return d
          })
          .attr('class', 'textselected')
          .style('text-anchor', 'start')
          .style('font-size', 12)

        //Now add title
        d3.select('chart').select('.legend')
          .append('text')
          .text(this.currentColor)
          .style('font-size', 12)

      }
      else {
        legend           = d3.select('chart')
          .select('g')
          .append('g')
          .attr('class', 'legend')
          .attr('transform', 'translate(' + (width - 10 * margin.horizontal) + ', 10)')
        // we need the min/max value and the color range.  Let's also round.
        var minHeatValue = Math.round(colorScale.domain()[0])
        var midHeatValue = Math.round(colorScale.domain()[1])
        var maxHeatValue = Math.round(colorScale.domain()[2])
        var minHeatColor = colorScale.range()[0]
        var midHeatColor = colorScale.range()[1]
        var maxHeatColor = colorScale.range()[2]
        // TODO: Refactor all of this to map the domain values and loop through
        legend.append('text')
          .attr('x', 0)
          .attr('y', 0)
          .text('Expression value')
          .style('font-size', 12)
        legend.append('rect')
          .attr('x', 20)
          .attr('y', 10)
          .attr('width', 20)
          .attr('height', 10)
          .style('fill', minHeatColor)
        legend.append('text')
          .attr('x', 50)
          .attr('y', 20)
          .text(minHeatValue)
          .attr('class', 'text')
          .style('text-anchor', 'start')
          .style('font-size', 12)
        legend.append('rect')
          .attr('x', 20)
          .attr('y', 20)
          .attr('width', 20)
          .attr('height', 10)
          .style('fill', midHeatColor)
        legend.append('text')
          .attr('x', 50)
          .attr('y', 30)
          .text(midHeatValue)
          .attr('class', 'text')
          .style('text-anchor', 'start')
          .style('font-size', 12)
        legend.append('rect')
          .attr('x', 20)
          .attr('y', 30)
          .attr('width', 20)
          .attr('height', 10)
          .style('fill', maxHeatColor)
        legend.append('text')
          .attr('x', 50)
          .attr('y', 40)
          .text(maxHeatValue)
          .attr('class', 'text')
          .style('text-anchor', 'start')
          .style('font-size', 12)

      }
      d3.select('.legend').call(d3.behavior.drag()    //Add drag behavior to legend
        .on('drag', function () {
            //Update the current position
            var x                    = d3.event.x
            var y                    = d3.event.y
            var estimatedLegendWidth = 100

            var figureXBorder = width - margin.horizontal - estimatedLegendWidth

            var xadj = Math.min(x, figureXBorder)
            if (xadj < 0) {
              xadj = 0
            }

            var yadj = Math.min(y, (height - margin.bottom))

            if (yadj < 0) {
              yadj = 0
            }

            d3.select(this).attr('transform', 'translate(' + xadj + ',' + yadj + ')')
          },
        ),
      )
    },

    /**
     * Build an array with lists of biomaterials sorted by value.
     * if the color param is passed in as "color" it will build the domain from
     * the color property instead of the sort property.
     * @returns {Array}
     */
    buildPropertyValuesDomain: function (color) {
      if (color === 'color') {
        this.currentSortingProperty = $('#propertyColorMenu').find(':selected').text()
      }
      else {
        this.currentSortingProperty = $('#propertySortMenu').find(':selected').text()
      }

      var list = []

      this.heatMap.map(function (biomaterial) {
        // var name = biomaterial.name;
        var propertyValue = biomaterial.properties[this.currentSortingProperty]
        if (!propertyValue) {
          propertyValue = 'Not set'
        }
        list.push(propertyValue)
      }.bind(this))

      // Return unique values only
      return $.grep(list, function (v, i) {
        return $.inArray(v, list) === i
      })
    },
  }
})(jQuery)
