<?php
$node = $variables['node'];
$protocol = $variables['node']->protocol;
$protocol = chado_expand_var($protocol,'field','protocol.protocoldescription'); ?>

<div class="tripal_protocol-teaser tripal-teaser">
  <div class="tripal-protocol-teaser-title tripal-teaser-title"><?php
    print l($node->title, "node/$node->nid", array('html' => TRUE));?>
  </div>
  <div class="tripal_protocol-teaser-text tripal-teaser-text"><?php
    print substr($protocol->protocoldescription, 0, 650);
    if (strlen($protocol->protocoldescription > 650)) {
      print "... " . l("[more]", "node/$node->nid");
    } ?>
  </div>
</div>

