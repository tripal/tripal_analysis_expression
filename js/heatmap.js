(function($){
  Drupal.behaviors.tripal_analysis_expression = {
    attach: function(context, settings){
      $(document).ready(function(){
        this.settings = settings.tripal_analysis_expression:
        if(! this.settings){ return };
        var heatmap_data = eval(this.settings.heatmap_data);
        //var layout       = eval(settings.tripal_analysis_expression.heatmap_layout);
        var left_margin = this.settings.left_margin;
        var bottom_margin = this.settings.bottom_margin;
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
