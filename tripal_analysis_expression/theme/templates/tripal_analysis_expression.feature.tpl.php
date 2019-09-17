<p><strong>Available Analyses</strong></p>
<ul>
    <?php foreach ($variables['analyses'] as $analysis) : ?>
    <li><?php print l($analysis->name, 'bio_data/'.chado_get_record_entity_by_table('analysis', $analysis->analysis_id)) ?></li>
    <?php endforeach; ?>
</ul>

<p><strong>Select an Expression Analysis</strong></p>

<select id="analyses-dropdown">
  <?php foreach ($variables['analyses'] as $analysis) : ?>
      <option value="<?php print $analysis->analysis_id; ?>">
        <?php print $analysis->name; ?>
      </option>
  <?php endforeach; ?>
</select>

<p id="propertySortDiv">
    <strong>Select a property to group and sort biological samples</strong>
</p>

<p id="propertyColorDiv">
    <strong>Select a property to color biological samples</strong>
</p>

<p>
    Hover the mouse over a column in the graph to view more information about
    that biological sample.
    You can click and drag to rearrange groups along the x-axis. You can also
    click and drag to move the legend.
</p>

<p>
    <a href="javascript:;" id="show-non-zero-only">Only Non-Zero Values</a> |
    <a href="javascript:;" id="reset-expression-plot">Reset</a>
</p>

<script type="text/javascript">
  Drupal.behaviors.tripal_analysis_expression = {
    attach: function (context, settings) {

    }
  };
</script>

<figure id="analysis-expression-figure"></figure>

  <a href="/tripal/analysis-expression/download?feature_ids=<?php print $variables['feature_id'] ?>&analysis_id=<?php print $variables['default_analysis_id']; ?>"
     id="expressionDownloadLink">
    Download expression dataset for this feature
  </a>
