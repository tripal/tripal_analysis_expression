<?php
$node = $variables['node'];
$analysis = $variables['node']->analysis;
$analysis = chado_expand_var($analysis, 'table', 'analysisprop', ['return_array' => 1]);

$properties = $analysis->analysisprop;

$expression = $node->analysis->tripal_analysis_expression;

?>
<?php
if ($variables['assay_protocol'] != '' or $variables['acquisition_protocol'] != '' or $variables['quantification_protocol'] != 0) { ?>

    <div class="tripal__analysis-data-block-desc tripal-data-block-desc">Protocols used in the process of the experiment.</div><?php

  $header = ['Protocol', 'Type', 'Date Run'];

  $rows = [];

  if ($variables['assay_protocol'] != '') {
    $rows[] = [
      $variables['assay_protocol'][0]->name,
      'Assay',
      preg_replace("/^(\d+-\d+-\d+) .*/", "$1", $variables['assaydate']),
    ];
  }
  if ($variables['acquisition_protocol'] != '') {
    $rows[] = [
      $variables['acquisition_protocol'][0]->name,
      'Acquistion',
      preg_replace("/^(\d+-\d+-\d+) .*/", "$1", $variables['acquisitiondate']),
    ];
  }
  if ($variables['quantification_protocol'] != '') {
    $rows[] = [
      $variables['quantification_protocol'][0]->name,
      'Quantification',
      preg_replace("/^(\d+-\d+-\d+) .*/", "$1", $variables['quantificationdate']),
    ];
  }

  if (!empty($rows)) {
    $table = [
      'header' => $header,
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal_protocol-table-properties',
        'class' => 'tripal_data_table',
      ],
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => [],
      'empty' => '',
    ];
    print theme_table($table);
  }
}
?>

