<?php
$node = $variables['node'];
$analysis = $variables['node']->analysis;
$analysis = chado_expand_var($analysis,'field','analysis.description'); ?>

<div class="tripal_analysis-teaser tripal-teaser">
  <div class="tripal-analysis-teaser-title tripal-teaser-title"><?php
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal_analysis-teaser-text tripal-teaser-text"><?php
    print substr($analysis->description, 0, 650);
    if (strlen($analysis->description > 650)) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>

