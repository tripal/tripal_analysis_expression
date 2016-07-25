(function($){
  Drupal.behaviors.tripal_analysis_expression = {
    attach: function(context, settings){
      $(document).ready(function(){
        /*
        var x = settings.tripal_analysis_expression.heatmap_x;
        var y = settings.tripal_analysis_expression.heatmap_y;
        var z = settings.tripal_analysis_expression.heatmap_z;
        var type = 'heatmap';
        var heatmap_data = [{x, y, z, type}];
        */
        var heatmap_data = eval(settings.tripal_analysis_expression.heatmap_data); 
        var layout = {
                title: 'Expression Heatmap',
                /*
                xaxis: {
                  title: 'Treatment'
                },
                */
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
