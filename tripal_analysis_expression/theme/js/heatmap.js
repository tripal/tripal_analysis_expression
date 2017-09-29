(function ($) {
  Drupal.behaviors.tripal_analysis_expression = {
    attach: function (context, settings) {
      $(function () {
        if (typeof settings.tripal_analysis_expression === 'undefined') {
          return;
        }
        else if (typeof settings.tripal_analysis_expression.heatmap_data === 'undefined') {
          return;
        }

        var node = Plotly.d3.select('#vis_expression').style({
          width: '100%'
        }).node();

        var heatmap_data  = settings.tripal_analysis_expression.heatmap_data;
        var left_margin   = settings.tripal_analysis_expression.left_margin;
        var bottom_margin = settings.tripal_analysis_expression.bottom_margin;
        var layout        = {
          title : 'Expression Heatmap',
          margin: {
            l: left_margin,
            b: bottom_margin
          }
        };
        Plotly.plot(node, heatmap_data, layout);

        $(window).on('resize', function () {
          Plotly.Plots.resize(node);
        });
      });
    }
  };
})(jQuery);
