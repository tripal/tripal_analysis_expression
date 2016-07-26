(function($){
  Drupal.behaviors.tripal_analysis_expression = {
    attach: function(context, settings){
      $(document).ready(function(){
        var heatmap_data = eval(settings.tripal_analysis_expression.heatmap_data); 
        //var layout       = eval(settings.tripal_analysis_expression.heatmap_layout);
        var layout = {
                title: 'Expression Heatmap',
                xaxis: {
                  title: 'Treatment'
                },
                margin: {
                  b: 100,
                  l: 400
                }     
            }
        Plotly.newPlot('vis_expression', heatmap_data, layout);
      })
    }
  }
})(jQuery);
