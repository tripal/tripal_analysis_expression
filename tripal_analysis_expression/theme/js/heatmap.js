(function ($) {
    Drupal.behaviors.tripal_analysis_expression = {
        attach: function (context, settings) {
            var analyses_options = settings.tripal_analysis_expression.heatmap_data;

            var select = Plotly.d3.select('#select_analysis').append("select").attr("id", "analysis_selector")
            Object.keys(analyses_options).map(function (d, i) {
                console.log(d)
                select.append("option")
                    .attr("Value", d)
                    .text(d)
            })
            select.change(function () {//when the selector changes, rebuild the plot
                    $(function () {
                        this.setup(settings, heatmap_data);
                    }.bind(this));
                }
            )
            $(function () {
                this.setup(settings, heatmap_data);
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
            var selectedAnalysis = Plotly.d3.select('#analysis_selector').find(":selected").text()
            var heatmap_data = settings.tripal_analysis_expression.heatmap_data[selectedAnalysis]
            var left_margin = settings.tripal_analysis_expression.left_margin;
            var bottom_margin = settings.tripal_analysis_expression.bottom_margin;
            var layout = {
                title: 'Expression Heatmap',
                margin: {
                    l: left_margin,
                    b: bottom_margin
                }
            };

            Plotly.plot(node, heatmap_data, layout);

            $(window).on('resize', function () {
                Plotly.Plots.resize(node);
            });
        }
    };
})(jQuery);
