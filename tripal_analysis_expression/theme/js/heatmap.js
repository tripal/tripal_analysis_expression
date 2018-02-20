(function ($) {
  // To make draggable:
  // Listening to plotly_click and then calling the appropriate restyle call
  // should do the trick.
  Drupal.behaviors.tripal_analysis_expression = {
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

      this.setup();
    },

    updateDownloadLink: function () {
      // Get analysis and build link for download
      var feature_ids = Object.keys(this.data.data[this.selectedAnalysis]).map(function (key) {
        return key;
      }).join(',');

      var link = this.settings.baseURL + '/tripal/analysis-expression/download?feature_ids=' + feature_ids + '&analysis_id=' + this.selectedAnalysis;
      $('#heatmap_download').attr('href', link);
    },

    constructHeatMapData: function () {
      if (typeof this.cache[this.selectedAnalysis] !== 'undefined') {
        return this.cache[this.selectedAnalysis];
      }

      var expression = this.data;
      var _that = this;
      // Extract data, features and biomaterials for the current analysis
      var data = {
        biomaterials: expression.biomaterials[this.selectedAnalysis],
        features: expression.features[this.selectedAnalysis],
        matrix: expression.data[this.selectedAnalysis]
      };

      var features = Object.keys(data.features).map(function (key) {
        return data.features[key];
      }.bind(this));

      var biomaterials = Object.keys(data.biomaterials).map(function (key) {
        return data.biomaterials[key].name;
      }.bind(this));

      var values = [];
      var tooltip = [];

      Object.keys(data.features).map(function (feature_id) {
        var row = [];
        var text = [];
        Object.keys(data.biomaterials).map(function (biomaterial_id) {
          if (typeof data.matrix[feature_id][biomaterial_id] !== 'undefined') {
            row.push(parseFloat(data.matrix[feature_id][biomaterial_id]));
          }
          else {
            row.push(0);
          }

          text.push(_that.formatTooltipEntry(data.biomaterials[biomaterial_id]));
        });

        values.push(row);
        tooltip.push(text);
      });

      this.cache[this.selectedAnalysis] = {
        x: biomaterials,
        y: features,
        z: values,
        text: tooltip
      };

      return this.cache[this.selectedAnalysis];
    },

    formatTooltipEntry: function (biomaterial) {
      var props = 'None available';
      if (biomaterial.props) {
        props = Object.keys(biomaterial.props).map(function (key) {
          var prop = biomaterial.props[key];
          return prop.name + ': ' + prop.value;
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
        _that.draw(_that.constructHeatMapData());
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

      this.draw(this.constructHeatMapData());
    },

    /**
     * Create the heat map
     */
    draw: function (data) {
      var left_margin = (this.data.maxLengths.feature * 14) / 2;
      var bottom_margin = (this.data.maxLengths.biomaterial * 14) / 2;
      var layout = {
        title: this.data.analyses[this.selectedAnalysis] + ' Expression',
        margin: {
          l: left_margin,
          b: bottom_margin
        }
      };

      var chartData = data;
      chartData.type = 'heatmap';

      Plotly.newPlot('expression_heat_map_canvas', [chartData], layout);
    }
  };
})(jQuery);
