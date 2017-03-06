<?php
$node = $variables['node'];
$biomaterial = $variables['node']->biomaterial;
$biomaterial = chado_expand_var($biomaterial,'field','biomaterial.description'); ?>

<div class="tripal_biomaterial-teaser tripal-teaser">
  <div class="tripal-biomaterial-teaser-title tripal-teaser-title"><?php
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal_biomaterial-teaser-text tripal-teaser-text"><?php
    print substr($biomaterial->description, 0, 650);
    if (strlen($biomaterial->description > 650)) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>

