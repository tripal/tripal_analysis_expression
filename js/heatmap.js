(function($){
  Drupal.behaviors.tripal_analysis_expression = {
    attach: function(context, settings){
      $(document).ready(function(){
        var heatmap_data = eval(settings.tripal_analysis_expression.heatmap_data); 
        //var layout       = eval(settings.tripal_analysis_expression.heatmap_layout);
        var left_margin = settings.tripal_analysis_expression.left_margin;
        var bottom_margin = settings.tripal_analysis_expression.bottom_margin;
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
