(function($){
  Drupal.behaviors.tripal_analysis_expression = {
    attach: function(context, settings){
      $(document).ready(function(){
        var config = settings.tripal_analysis_expression:
        if(! config){ return };
        var heatmap_data = eval(config.heatmap_data);
        //var layout       = eval(settings.tripal_analysis_expression.heatmap_layout);
        var left_margin = config.left_margin;
        var bottom_margin = config.bottom_margin;
        var layout = {
                title: 'Expression Heatmap',
                /*
                xaxis: {
                  title: bottom_margin
                },
                */
                margin: {
                  b: bottom_margin,
                  l: left_margin
                }
            }
        Plotly.newPlot('vis_expression', heatmap_data, layout);
      })
    }
  }
})(jQuery);
