<?php
$node = $variables['node'];
$arraydesign = $variables['node']->arraydesign;
$arraydesign = chado_expand_var($arraydesign,'field','arraydesign.description'); ?>

<div class="tripal_arraydesign-teaser tripal-teaser">
  <div class="tripal-arraydesign-teaser-title tripal-teaser-title"><?php
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal_arraydesign-teaser-text tripal-teaser-text"><?php
    print substr($arraydesign->description, 0, 650);
    if (strlen($arraydesign->description > 650)) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>

