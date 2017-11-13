//To make draggable:
//Listening to plotly_click and then calling the appropriate restyle call should do the trick.

(function ($) {
    Drupal.behaviors.tripal_analysis_expression = {
        attach: function (context, settings) {
            var analyses_options = settings.tripal_analysis_expression.heatmap_data;

            var select = Plotly.d3.select('#select_analysis')
                .append("select")
                .attr("id", "analysis_selector")

            Object.keys(analyses_options).map(function (d, i) {
                select.append("option")
                    .attr("Value", d)
                    .text(d)
            })
            $(function () {
                this.setup(settings);
            }.bind(this));
        },

        setup: function (settings, heatmap_data) {
            if (typeof settings.tripal_analysis_expression === 'undefined') {
                return;
            }
            else if (typeof settings.tripal_analysis_expression.heatmap_data === 'undefined') {
                return;
            }

            var node = Plotly.d3.select('#vis_expression').style({
                width: '100%'
            }).node();

            //get analysis selected and corresponding heatmap data
            var selectedAnalysis = $('#analysis_selector').find(":selected").text()
            var heatmap_data = settings.tripal_analysis_expression.heatmap_data[selectedAnalysis]
            var left_margin = settings.tripal_analysis_expression.left_margin;
            var bottom_margin = settings.tripal_analysis_expression.bottom_margin;
            var layout = {
                title: selectedAnalysis + "Expression",
                margin: {
                    l: left_margin,
                    b: bottom_margin
                }
            };


            Plotly.plot(node, heatmap_data, layout);


            // https://plot.ly/javascript/dropdowns/
            //alternatively try using a plotly based dropdown instead
            $("#select_analysis").change(function () {//when the selector changes, rebuild the plot
                    var node = Plotly.d3.select('#vis_expression').style({
                        width: '100%'
                    }).node();
                    var heatmap_data = settings.tripal_analysis_expression.heatmap_data[selectedAnalysis]

                  //  Plotly.restyle(node, heatmap_data, layout);
                Plotly.replot(node)

                }
            ).bind(this)

            $(window).on('resize', function () {
                Plotly.Plots.resize(node);
            });
        }
    };
})(jQuery);
