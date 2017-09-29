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

        var heatmap_data  = settings.tripal_analysis_expression.heatmap_data;
        var left_margin   = settings.tripal_analysis_expression.left_margin;
        var bottom_margin = settings.tripal_analysis_expression.bottom_margin;
        var layout        = {
          title : 'Expression Heatmap',
          margin: {
            b: bottom_margin,
            l: left_margin
          }
        };
        Plotly.newPlot('vis_expression', heatmap_data, layout);
      });
    }
  };
})(jQuery);
