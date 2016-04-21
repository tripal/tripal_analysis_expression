<?php

$organism = $variables['node']->organism;

if (sizeof($variables['biomaterial_ids']) > 0) {
  $element = 1;
  $num_per_page = 10;
 
  $biomaterials = array();
  $values = array();
  foreach ($variables['biomaterial_ids'] as $biomaterial_id) {
    $values = array('biomaterial_id' => $biomaterial_id);
    $options = array(
      'include_fk' => array(
        'type_id' => 1
      )
    );
    $biomaterials[] = chado_generate_var('biomaterial', $values);
  } ?>
    <div class="tripal_organism-data-block-desc tripal-data-block-desc">The following browser provides a list of biomaterials associated with this organism.</div> <?php

  $headers = array('Biomaterial Name', 'Organism', 'Biomaterial Provider');
  $rows = array();

  foreach($biomaterials as $biomaterial) {
    $bname = $biomaterial->name;
    $borganism = '';
    $bcontact = '';

    if (property_exists($biomaterial, 'nid')) {
      $bname = l($bname, 'node/' . $biomaterial->nid, array('attributes' => array('target' => '_blank')));
    }

    if ($biomaterial->taxon_id) {
      if (property_exists($biomaterial->taxon_id, 'nid')) {
        $bgenus = $biomaterial->taxon_id->genus;
        $bspecies = $biomaterial->taxon_id->species;
        $bcommon_name = $biomaterial->taxon_id->common_name;
        $borganism = l($bgenus . ' ' . $bspecies . ' (' . $bcommon_name . ')', 'node/' . $biomaterial->taxon_id->nid, array('attributes' => array('target' => '_blank')));
      }
      else {
        $bgenus = $biomaterial->taxon_id->nid;
        $bspecies = $biomaterial->taxon_id->species;
        $bcommon_name = $biomaterial->taxon_id->common_name;
        $borganism = $bgenus . ' ' . $bspecies . ' (' . $bcommon_name . ')';
      }
    }

    if ($biomaterial->biosourceprovider_id) {
      if (property_exists($biomaterial->biosourceprovider_id,'nid')) { 
        $bcontact = l($biomaterial->biosourceprovider_id->name, 'node/' . $biomaterial->biosourceprovider_id->nid, array('attributes' => array('target' => '_blank')));
      }
      else {
        $bcontact = $biomaterial->biosourceprovider_id->name;
      }
    }

    $rows[] = array(
      $bname,
      $borganism,
      $bcontact,
    );
  }

  $current_page = pager_default_initialize(count($rows), $num_per_page, 1);
  $chunks = array_chunk($rows, $num_per_page, TRUE);
  $output = theme('table', array('header' => $headers, 'rows' => $chunks[$current_page]));
  $output .= theme('pager', array('quantity' => 5, 'element' => $element, 'parameters' => array('block' => 'biomaterial_browser')));

  print $output;

}
 
