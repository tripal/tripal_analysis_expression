//To make draggable:
//Listening to plotly_click and then calling the appropriate restyle call should do the trick.

(function ($) {
    Drupal.behaviors.tripal_analysis_expression = {
        attach: function (context, settings) {
            // var analyses_options = settings.tripal_analysis_expression.heatmap_data;

            //========= only data from one analysis is available. Add some fake data to show the change when drop down value changes =====
            var hd = settings.tripal_analysis_expression.heatmap_data;
            // var analyses_options = {
            //     'That one time we created biomaterials and expression': hd['That one time we created biomaterials and expression'],
            //     'second analysis': [{
            //         hoverinfo: 'all',
            //         text: [
            //             ['a', 'b', 'c'],
            //             ['a', 'b', 'c'],
            //             ['a', 'b', 'c'],
            //             ['a', 'b', 'c'],
            //         ],
            //         x: ['label 1', 'label 2', 'label 3'],
            //         y: ['feature 1', 'feature 2', 'feature 3', 'feature 4'],
            //         z: [
            //             [1, 2, 4],
            //             [3, 1, 20],
            //             [3, 1, 20],
            //             [3, 10, 20],
            //         ]
            //     }]
            // };
            // //=======================================
            var analyses_options = hd;


            // build drop down UI
            var select = Plotly.d3.select('#select_analysis')
                .append("select")
                .attr("id", "analysis_selector")

            Object.keys(analyses_options).map(function (d) {
                select.append("option")
                    .attr("Value", d)
                    .text(d)
            })


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
                console.log(analyses_options)
                var heatmap_data = [analyses_options[selectedAnalysis][0]];
                console.log(heatmap_data)
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
    };
})(jQuery);
