<?php

$analysis = $variables['node']->analysis;
$results = $variables['associated_biomaterials'];

if (sizeof($results) > 0) {
  $element = 1;
  $num_per_page = 25;
 
  $biomaterials = array();
  foreach ($results as $result) {
    $values = array('biomaterial_id' => $result->biomaterial_id);
    $options = array(
      'include_fk' => array(
        'type_id' => 1
      )
    );
    $biomaterials[] = chado_generate_var('biomaterial', $values);
  } ?>
    <div class="tripal_analysis-data-block-desc tripal-data-block-desc">The following browser provides a list of biomaterials associated with this analysis.</div> <?php

  $headers = array('Biomaterial Name');
  $rows = array();

  foreach($biomaterials as $biomaterial) {
    $bname = $biomaterial->name;
    if (property_exists($biomaterial, 'nid')) {
      $bname = l($bname, "node/$biomaterial->nid", array('attributes' => array('target' => '_blank')));
    }
    $rows[] = array(
      $bname
    );
  }

  $num_per_page = 10;
  $current_page = pager_default_initialize(count($rows), $num_per_page, 1);
  $chunks = array_chunk($rows, $num_per_page, TRUE);
  $output = theme('table', array('header' => $headers, 'rows' => $chunks[$current_page]));
  $output .= theme('pager', array('quantity' => count($rows), 'element' => 1, 'parameters' => array('block' => 'biomaterial_browser')));

  print $output;

}
 
