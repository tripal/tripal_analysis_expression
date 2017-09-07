<?php

$analysis = $variables['node']->analysis;
//$biomaterial_ids = $variables['biomaterial_ids'];

if (sizeof($variables['biomaterial_ids']) > 0) {
  $element = 1;
  $num_per_page = 10;

  $biomaterials = [];
  foreach ($variables['biomaterial_ids'] as $biomaterial_id) {
    $values = ['biomaterial_id' => $biomaterial_id];
    $options = [
      'include_fk' => [
        'type_id' => 1,
      ],
    ];
    $biomaterials[] = chado_generate_var('biomaterial', $values);
    //$biomaterials[] = chado_expand_var($biomaterials, 'table', 'organism');
  } ?>
    <div class="tripal_analysis-data-block-desc tripal-data-block-desc">The following browser provides a list of biomaterials associated with this analysis.</div> <?php

  $headers = ['Biomaterial Name', 'Organism', 'Biomaterial Provider'];
  $rows = [];

  foreach ($biomaterials as $biomaterial) {
    $bname = $biomaterial->name;
    $borganism = '';
    $bcontact = '';

    if (property_exists($biomaterial, 'nid')) {
      $bname = l($bname, 'node/' . $biomaterial->nid, ['attributes' => ['target' => '_blank']]);
    }

    if ($biomaterial->taxon_id) {
      if (property_exists($biomaterial->taxon_id, 'nid')) {
        $bgenus = $biomaterial->taxon_id->genus;
        $bspecies = $biomaterial->taxon_id->species;
        $bcommon_name = $biomaterial->taxon_id->common_name;
        $borganism = l($bgenus . ' ' . $bspecies . ' (' . $bcommon_name . ')', 'node/' . $biomaterial->taxon_id->nid, ['attributes' => ['target' => '_blank']]);
      }
      else {
        $bgenus = $biomaterial->taxon_id->nid;
        $bspecies = $biomaterial->taxon_id->species;
        $bcommon_name = $biomaterial->taxon_id->common_name;
        $borganism = $bgenus . ' ' . $bspecies . ' (' . $bcommon_name . ')';
      }
    }

    if ($biomaterial->biosourceprovider_id) {
      if (property_exists($biomaterial->biosourceprovider_id, 'nid')) {
        $bcontact = l($biomaterial->biosourceprovider_id->name, 'node/' . $biomaterial->biosourceprovider_id->nid, ['attributes' => ['target' => '_blank']]);
      }
      else {
        $bcontact = $biomaterial->biosourceprovider_id->name;
      }
    }

    $rows[] = [
      $bname,
      $borganism,
      $bcontact,
    ];
  }

  $current_page = pager_default_initialize(count($rows), $num_per_page, 1);
  $chunks = array_chunk($rows, $num_per_page, TRUE);
  $output = theme('table', [
    'header' => $headers,
    'rows' => $chunks[$current_page],
  ]);
  $output .= theme('pager', [
    'quantity' => count($rows),
    'element' => $element,
    'parameters' => ['block' => 'biomaterial_browser'],
  ]);

  print $output;
}
 
