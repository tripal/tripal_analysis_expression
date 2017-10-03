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
      this.request          = null;
      this.reset();

      this.term_field.on('keyup', this.search.bind(this));

      var that = this;

      $(document).on('click', '.heatmap-results-item', function (event) {
        var link = $(this);

        if (link.is('.disabled')) {
          return;
        }

        that.chooseFeature(link.data('value'));
        link.addClass('disabled');
      });
    },

    reset: function () {
      this.results_block.html('<p>Type a search term above to search for features</p>');
    },

    loadingShow: function () {
      $('#heatmap-form-throbber').addClass('ajax-progress').addClass('ajax-progress-throbber');
    },

    loadingHide: function () {
      $('#heatmap-form-throbber').removeClass('ajax-progress').removeClass('ajax-progress-throbber');
    },

    search: function (event) {
      var terms = this.term_field.val();

      if (terms.length === 0) {
        return this.reset();
      }

      // Accept only alphanumeric characters and backspace
      var input = String.fromCharCode(event.keyCode);
      if (!/[a-zA-Z0-9-_ ]/.test(input) && event.keyCode !== 8) {
        return;
      }

      if (this.request !== null) {
        this.request.abort();
      }

      this.request = new window.XMLHttpRequest();

      this.loadingShow();
      $.ajax({
        url     : '/tripal/analysis-expression/heatmap/search',
        data    : {terms: terms},
        success : this.renderSearchResults.bind(this),
        error   : function (a, b, c) {
          if (a.status === 0) {
            return;
          }
          this.loadingHide();
          console.log('Search failed with status code: ' + a.status, b, c);
        }.bind(this),
        type    : 'get',
        dataType: 'json',
        xhr     : function () {
          return this.request;
        }.bind(this)
      });
    },

    renderSearchResults: function (response) {
      this.loadingHide();

      var html = '<p>No results found</p>';
      if (response.data.length > 0) {
        html = '<p class="heatmap-dropdown-header">Select Features</p>';
        html += response.data.map(this.renderRow.bind(this)).join('');
      }
      this.results_block.html(html);
    },

    renderRow: function (row) {
      var name = row.uniquename;

      var disabled = this.getFeatures().indexOf(name) > -1 ? ' disabled' : '';

      var html = '<a href="javascript:void(0);" class="heatmap-results-item' + disabled + '" data-value="' + name + '">'
          + name
          + '<div><small>Organism: ' + row.common_name + '</small></div>';

      if (row.accession) {
        html += '<div><small>Accession: ' + row.accession + '</small></div>';
      }

      html += '</a>';

      return html;
    },

    chooseFeature: function (feature) {
      var value = this.getFeatures();
      if (value.length === 0) {
        this.feature_textarea.val(feature);
        return;
      }
      value.push(feature);
      this.feature_textarea.val(value.join(',').trim());
    },

    getFeatures: function () {
      var value = this.feature_textarea.val().trim();
      if (value.length === 0) {
        return [];
      }

      return value.split(',').map(function (m) {
        return m.trim();
      });
    }
  };
})(jQuery);
