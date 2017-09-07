<?php
// This conditional is added to prevent errors in the biomaterial TOC admin page.
if (property_exists($variables['node'], 'biomaterial')) {
  $biomaterial = $variables['node']->biomaterial;
  $biomaterial = chado_expand_var($biomaterial, 'field', 'biomaterial.description');
  $ontology_terms = $variables['biomaterial_ontology_terms'];
  ?>

    <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc"></div>

  <?php

  $analysis_name = $variables['analysis_name'];
  $analysis_nid = $variables['analysis_nid'];

  $headers = [];
  $rows = [];

  // The biomaterial name.

  $rows[] = [
    [
      'data' => 'Biomaterial',
      'header' => TRUE,
      'width' => '20%',
    ],
    '<i>' . $biomaterial->name . '</i>',
  ];

  // The organism from which the biomaterial was collected
  if ($biomaterial->taxon_id) {
    $organism = '<i>' . $biomaterial->taxon_id->genus . ' ' . $biomaterial->taxon_id->species . '</i> (' . $biomaterial->taxon_id->common_name . ')';
    if (property_exists($biomaterial->taxon_id, 'nid')) {
      $organism = l('<i>' . $biomaterial->taxon_id->genus . ' ' . $biomaterial->taxon_id->species . '</i> (' . $biomaterial->taxon_id->common_name . ')', 'node/' . $biomaterial->taxon_id->nid, ['html' => TRUE]);
    }
    $rows[] = [
      [
        'data' => 'Organism',
        'header' => TRUE,
        'width' => '20%',
      ],
      $organism,
    ];
  }

  // The analysis
  if ($analysis_name) {
    $analysis = $analysis_name;
    if ($analysis_nid) {
      $analysis = l($analysis_name, 'node/' . $analysis_nid, ['html' => TRUE]);
    }
    $rows[] = [
      [
        'data' => 'Analysis',
        'header' => TRUE,
        'width' => '20%',
      ],
      $analysis,
    ];
  }

  // The biosource provider
  if ($biomaterial->biosourceprovider_id) {
    $rows[] = [
      [
        'data' => 'Biomaterial Provider',
        'header' => TRUE,
        'width' => '20%',
      ],
      '<i>' . $biomaterial->biosourceprovider_id->name . '</i>',
    ];
  }

  // allow site admins to see the biomaterial ID
  if (user_access('view ids')) {
    // Biomaterial ID
    $rows[] = [
      [
        'data' => 'Biomaterial ID',
        'header' => TRUE,
        'class' => 'tripal-site-admin-only-table-row',
      ],
      [
        'data' => $biomaterial->biomaterial_id,
        'class' => 'tripal-site-admin-only-table-row',
      ],
    ];
  }

  // Generate the table of data provided above. 
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_biomaterial-table-base',
      'class' => 'tripal-biomaterial-data-table tripal-data-table',
    ],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];

  // Print the table and the description.
  print theme_table($table);

  // Print the biomaterial description.
  if ($biomaterial->description) { ?>
      <div style="text-align: justify"><?php print $biomaterial->description ?></div> <?php
  }

  $first = 0;

  foreach ($ontology_terms as $ontology => $ontology_term) {
    if (count($ontology_term) == 0) {
      continue;
    }

    if ($first != 0) {
      print '<br>';
    }

    print '<br><b>' . ucfirst(str_replace('_', ' ', $ontology)) . ':</b>';

    foreach ($ontology_term as $term) {
      print "&nbsp&nbsp&nbsp&nbsp";
      print l(t(str_replace('_', ' ', $term)), "/biomaterial-ontology-terms/" . $ontology . "/" . $term);
    }

    $first = 1;
  }

  ?>


  <?php
}
?>

