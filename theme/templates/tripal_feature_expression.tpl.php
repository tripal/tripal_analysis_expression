<?php

$feature = $variables['node']->feature;

if (!$variables['has_exp'] and $variables['json_exp']) { ?>

    No biomaterial libraries express this feature.

<?php }

else {
  if ($variables['json_exp']) {
    ?>
      <a name="expression-top"> </a>
      <p>
          Hover the mouse over a column in the graph to view expression values.
          <br>
          <a href=''
             onclick="expSortDown() return false;">Sort Descending</a> |
          <a href='' onclick="expSortUp() return false;">Sort Ascending</a> |
          <a href=''
             onclick="nonZero() return false;">Only Non-Zero Values</a> |
          <a href='' onclick="expChart() return false;">Tile/Chart</a> |
          <a href='' onclick="expNormal() return false;">Reset</a>
      </p>

    <?php

    tripal_add_d3js();
    $hide_biomaterial_labels = $variables['hide_biomaterial_labels'];
    $json_exp = $variables['json_exp'];
    $limit_label_length = $variables['limit_label_length'];
    $expression_display = $variables['expression_display'];
    $biomaterial_display_width = $variables['biomaterial_display_width']; ?>

      <script type="text/javascript">
        <?php
        print 'var heatMapRaw=' . $json_exp . ';';
        print 'var maxLength=' . $limit_label_length . ';';
        print 'var showLabels=' . $hide_biomaterial_labels . ';';
        print 'var col="' . $expression_display . '";';
        print 'var colWidth=' . $biomaterial_display_width . ';';
        ?>
        Drupal.behaviors.tripal_analysis_expression = {
          attach: function (context, settings) {
            expNormal();
          }
        };
      </script>

      <figure>

      </figure>

      <a href="#expression-top">back to top</a>


    <?php

  }
}
