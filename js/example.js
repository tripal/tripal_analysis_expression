(function ($) {
  Drupal.behaviors.tripal_analysis_expression_example = {
    attach: function (context, settings) {
      var example = 'Fraxinus_pennsylvanica_120313_comp59663_c0_seq1, Fraxinus_pennsylvanica_120313_comp59663_c0_seq2, Fraxinus_pennsylvanica_120313_comp56723_c0_seq1, Fraxinus_pennsylvanica_120313_comp60325_c0_seq16, Fraxinus_pennsylvanica_120313_comp61194_c0_seq4, Fraxinus_pennsylvanica_120313_comp61194_c0_seq5, Fraxinus_pennsylvanica_120313_comp61194_c0_seq8, Fraxinus_pennsylvanica_120313_comp61194_c0_seq6'
      $('#edit-example-button').click(function (e) {
        e.preventDefault()
        $(this).val('Creating example heat map. Please wait...')
        $('#edit-heatmap-feature-uniquename').val(example)
        $(this).parents('form').submit()
      })
    }
  }
})(jQuery)