(function ($) {
  Drupal.behaviors.tripal_analysis_expression_heatmap_search = {
    attach: function (context, settings) {
      $(function () {
        this.setupSearch();
      }.bind(this));
    },

    setupSearch: function () {
      this.term_field       = $('#heatmap-search_term');
      this.results_block    = $('#feature-heatmap-search-results');
      this.feature_textarea = $('#heatmap_feature_uniquename');
      this.reset();

      this.term_field.on('keyup', this.search.bind(this));
      $(document).on('click', '.heatmap-results-item', function (event) {
        var link = $(event.target);

        if (link.is('.disabled')) {
          return;
        }

        this.chooseFeature(link.data('value'));
        $(event.target).addClass('disabled');
      }.bind(this));
    },

    reset: function () {
      this.results_block.html('<p>Type a search term above to search for features</p>');
    },

    search: function () {
      var terms = this.term_field.val();

      if (terms.length === 0) {
        return this.reset();
      }

      $.get('/tripal/analysis-expression/heatmap/search', {
        terms: terms
      }, this.renderSearchResults.bind(this), 'json').error(function (a, b, c) {
        console.log('Search failed with status code: ' + a.status, b, c);
      });
    },

    renderSearchResults: function (response) {
      var html = '<p>No results found</p>';
      if (response.data.length > 0) {
        html = response.data.map(this.renderRow.bind(this)).join('');
      }
      this.results_block.html(html);
    },

    renderRow: function (row) {
      var name = row.uniquename;

      var disabled = this.feature_textarea.val().indexOf(name) > -1 ? ' disabled' : '';

      return '<a href="javascript:void(0);" class="heatmap-results-item' + disabled + '" data-value="' + name + '">'
          + name
          + '</a>';
    },

    chooseFeature: function (feature) {
      var value = this.feature_textarea.val().trim();
      if (value.length === 0) {
        this.feature_textarea.val(feature);
        return;
      }

      var selection = value.split(',');
      selection.push(feature);
      this.feature_textarea.val(selection.join(',').trim());
    }
  };
})(jQuery);
