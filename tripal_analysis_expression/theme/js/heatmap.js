(function ($) {
  // To make draggable:
  // Listening to plotly_click and then calling the appropriate restyle call
  // should do the trick.
  Drupal.behaviors.tripal_analysis_expression = {
    /**
     * Auto triggered by Drupal when the document is ready.
     *
     * @param context
     * @param settings
     */
    attach: function (context, settings) {
      this.settings = settings.tripal_analysis_expression;
      this.cache = {};
      if (!this.settings) {
        return;
      }

      this.data = this.settings.data;

      // Build drop down UI
      var $selectContainer = $('#select_analysis');
      this.$select = $('<select />');
      $selectContainer
          .append(this.$select)
          .attr('id', 'analysis_selector');

      Object.keys(this.data.analyses).map(function (key) {
        var $option = $('<option />', {
          value: key
        }).text(this.data.analyses[key]);
        this.$select.append($option);
      }.bind(this));

      this.selectedAnalysis = parseInt(this.$select.val());

      // Set the download link
      this.updateDownloadLink();

      // Add the properties sort dropdown
      this.createPropertiesDropdown();

      this.setup();
    },

    /**
     * Deals with changes to the analysis dropdown
     * to update the download link
     */
    updateDownloadLink: function () {
      // Get analysis and build link for download
      var feature_ids = Object.keys(this.data.data[this.selectedAnalysis]).map(function (key) {
        return key;
      }).join(',');

      var link = this.settings.baseURL;
      link += '/tripal/analysis-expression/download?feature_ids=' + feature_ids;
      link += '&analysis_id=' + this.selectedAnalysis;
      $('#heatmap_download').attr('href', link);
    },

    /**
     * Create the sort by property dropdown
     * and append to the page.
     */
    createPropertiesDropdown: function () {
      // Create the dropdown
      this.$propsSelect = $('<select />');

      // Add the dropdown to the page
      $('#select_props').append(this.$propsSelect);

      // Initialize the props
      this.updatePropsDropdown();

      this.$propsSelect.on('change', function (e) {
        this.selectedProp = e.target.value;
        this.draw(this.constructHeatMapData());
      }.bind(this));
    },

    /**
     * Respond to changes in analysis selector
     */
    updatePropsDropdown: function () {
      this.$propsSelect.html('');
      var props = this.data.properties[this.selectedAnalysis];
      Object.keys(props).map(function (key) {
        var option = $('<option />', {
          value: key
        }).text(props[key]);

        this.$propsSelect.append(option);
      }.bind(this));

      this.selectedProp = this.$propsSelect.val() || false;
      this.draw(this.constructHeatMapData());
    },

    /**
     * Creates the heat map data structure
     *
     * @return {Object}
     */
    constructHeatMapData: function () {
      var expression = this.data;
      var sortBy = this.selectedProp;

      // Extract data, features and biomaterials for the current analysis
      var data = {
        biomaterials: expression.biomaterials[this.selectedAnalysis],
        features: expression.features[this.selectedAnalysis],
        matrix: expression.data[this.selectedAnalysis]
      };

      // Sort by selected prop if available
      var biomaterialKeys = Object.keys(data.biomaterials);
      if (sortBy) {
        biomaterialKeys.sort(function (a, b) {
          var props1 = data.biomaterials[a].props[sortBy];
          var props2 = data.biomaterials[b].props[sortBy];

          // Both biomaterials have the prop
          if (props1 && props2) {
            return this.compareByType(props1.value, props2.value);
          }

          // Only the first biomaterial has the prop
          if (props1) {
            return -1;
          }

          // Only the second biomaterial has the prop
          if (props2) {
            return 1;
          }

          // They both do not have the prop
          return 0;
        }.bind(this));
      }

      // Create the X axis
      var biomaterials = biomaterialKeys.map(function (biomaterial_id) {
        return data.biomaterials[biomaterial_id].name || '';
      }.bind(this));

      // Get the Y axis
      var features = Object.keys(data.features).map(function (key) {
        return data.features[key];
      }.bind(this));

      var values = [];
      var tooltip = [];

      Object.keys(data.features).map(function (feature_id) {
        var row = [];
        var text = [];
        biomaterialKeys.map(function (biomaterial_id) {
          if (typeof data.matrix[feature_id][biomaterial_id] !== 'undefined') {
            row.push(parseFloat(data.matrix[feature_id][biomaterial_id]));
          }
          else {
            row.push(0);
          }

          text.push(this.formatTooltipEntry(data.biomaterials[biomaterial_id], sortBy));
        }.bind(this));

        values.push(row);
        tooltip.push(text);
      }.bind(this));

      return [{
        x: biomaterials,
        y: features,
        z: values,
        text: tooltip,
        type: 'heatmap'
      }];
    },

    /**
     * Add information to the tooltip.
     *
     * @param biomaterial
     * @return {string}
     */
    formatTooltipEntry: function (biomaterial, sortBy) {
      var props = 'None available';
      if (biomaterial.props) {
        props = Object.keys(biomaterial.props).map(function (key) {
          var prop = biomaterial.props[key];
          var label = prop.name;
          if (key === sortBy) {
            label = '<span style="font-weight: bold; color: red">' + label + '</span>';
            console.log(label);
          }
          return label + ': ' + prop.value;
        }).join('<br />');
      }

      return [
        'Name: ' + biomaterial.name,
        'Collected By:' + biomaterial.contact,
        'Description: ' + biomaterial.description,
        'Properties:',
        props
      ].join('<br />');
    },

    /**
     * Set the scene and create the heat map
     */
    setup: function () {
      var _that = this;
      this.$select.on('change', function () {
        _that.selectedAnalysis = parseInt($(this).val());
        _that.updatePropsDropdown.call(_that);
        _that.updateDownloadLink.call(_that);
      });

      var node = Plotly.d3.select('#expression_heat_map_canvas').style({
        width: '100%'
      }).node();

      var download_link = $('#heatmap_download');
      var download_message = $('<span />');
      download_link.after(download_message);
      download_link.click(function (e) {
        e.preventDefault();
        var src = $(this).attr('href');
        download_message.html(' Creating file. Download will start automatically ...');
        var iframe = $('<iframe />', {
          src: src,
          width: 1,
          height: 1
        });
        $('body').append(iframe);
      });

      $(window).on('resize', function () {
        Plotly.Plots.resize(node);
      });
    },

    /**
     * Draws the heat map using the given data.
     *
     * @param {Object} data
     */
    draw: function (data) {
      var left_margin = (this.data.maxLengths.feature * 14) / 2;
      var bottom_margin = (this.data.maxLengths.biomaterial * 16) / 2;
      var layout = {
        title: this.data.analyses[this.selectedAnalysis] + ' Expression',
        margin: {
          l: left_margin,
          b: bottom_margin
        }
      };

      Plotly.newPlot('expression_heat_map_canvas', data, layout);
      $('#expression_heat_map_canvas').unbind('plotly_hover').on('plotly_hover', function (data) {
        $(".hovertext text").each(function(text) {
          text.css({
            'color': '#000'
          })
        });
      });
    },

    compareByType: function (a, b) {
      var v1, v2;

      if (typeof a === 'string') {
        // Ignore case
        v1 = a.toLowerCase();
      }
      else {
        v1 = a;
      }

      if (typeof b === 'string') {
        // Ignore case
        v2 = b.toLowerCase();
      }
      else {
        v2 = b;
      }

      if (v1 < v2) {
        return 1;
      }

      if (v1 > v2) {
        return -1;
      }

      return 0;
    }
  };
})(jQuery);
