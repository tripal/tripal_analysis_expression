<p>
    This analysis contains expression data
    for <?php print $variables['features_count']; ?> features
    across <?php print $variables['biomaterials_count']; ?> biosamples.
</p>

<p>
    <a href="<?php print file_create_url($variables['file']) ?>"
       id="expressionDownloadLink">
        Download all expression data associated with this analysis.
    </a>
</p>

<script>
  (function ($) {
    $(function () {
      var message = $('<span />');
      var link = $('#expressionDownloadLink');
      link.after(message);
      link.click(function (e) {
        e.preventDefault();
        var src = $(this).attr('href');
        var iframe = $('<iframe />', {
          src: src,
          width: 1,
          height: 1
        });
        $('body').append(iframe);
        message.html('Generating file. Download will start automatically...');
      });
    });
  })(jQuery);
</script>