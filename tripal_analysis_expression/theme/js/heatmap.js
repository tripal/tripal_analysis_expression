//To make draggable:
//Listening to plotly_click and then calling the appropriate restyle call should do the trick.

(function ($) {
    Drupal.behaviors.tripal_analysis_expression = {
        attach: function (context, settings) {
            // var analyses_options = settings.tripal_analysis_expression.heatmap_data;

            //========= only data from one analysis is available. Add some fake data to show the change when drop down value changes =====
            var hd = settings.tripal_analysis_expression.heatmap_data;
            var features = settings.tripal_analysis_expression.feature_list;
            console.log(features)
            var analysis_legend = settings.tripal_analysis_expression.analysis_legend;

            var analyses_options = hd;


            console.log(analyses_options)

            // build drop down UI
            var select = Plotly.d3.select('#select_analysis')
                .append("select")
                .attr("id", "analysis_selector")

            Object.keys(analyses_options).map(function (d) {
                select.append("option")
                    .attr("Value", d)
                    .text(d)
            })

            //get analysis and build link for download
            var selectedAnalysis = document.querySelector('#analysis_selector').value;
            var link = '/tripal/analysis-expression/download?feature_ids=' + features[selectedAnalysis] + '&analysis_id=' + analysis_legend[selectedAnalysis]
            Plotly.d3.select('#heatmap_download')
                .attr("href", link)

            $(function () {
                this.setup(settings, analyses_options);
            }.bind(this));
        },

        setup: function (settings, analyses_options) {
            if (typeof settings.tripal_analysis_expression === 'undefined') {
                return;
            }
            else if (typeof settings.tripal_analysis_expression.heatmap_data === 'undefined') {
                return;
            }

            var node = Plotly.d3.select('#vis_expression').style({
                width: '100%'
            }).node();


            // by default, the fist analysis from the dropdown is used to plot.
            make_heatmap(Object.keys(analyses_options)[0]);

            function make_heatmap(selectedAnalysis) {
                var heatmap_data = [analyses_options[selectedAnalysis][0]];
                var left_margin = settings.tripal_analysis_expression.left_margin;
                var bottom_margin = settings.tripal_analysis_expression.bottom_margin;
                var layout = {
                    title: selectedAnalysis + " Expression",
                    margin: {
                        l: left_margin,
                        b: bottom_margin
                    }
                };

                Plotly.newPlot('vis_expression', heatmap_data, layout);

                var analysisSelector = document.querySelector('#analysis_selector');

                function update_heatmap() {
                    make_heatmap(analysisSelector.value);
                }

                analysisSelector.addEventListener('change', update_heatmap, false);

            }


            $(window).on('resize', function () {
                Plotly.Plots.resize(node);
            });
        }

        //Attach the download link

    };
})(jQuery);
