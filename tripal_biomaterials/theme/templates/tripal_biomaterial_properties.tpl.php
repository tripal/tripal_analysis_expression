<?php
// This line is added so that no errors show in the TOC admin page. 
if (property_exists($variables['node'], 'biomaterial')) {
  $biomaterial = $variables['node']->biomaterial;
  $biomaterial = chado_expand_var($biomaterial, 'table', 'biomaterialprop', ['return_array' => 1]);
  $properties = $biomaterial->biomaterialprop;

  // Check for properties.  
  if (count($properties) > 0) { ?>
      <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">Additional information about this biomaterial:</div><?php

    $headers = ['Property Name', 'Value'];

    $rows = [];
    foreach ($properties as $property) {
      $rows[] = [
        ucfirst(preg_replace('/_/', ' ', $property->type_id->name)),
        $property->value,
      ];
    }

    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal_biomaterial-table-properties',
        'class' => 'tripal-data-table',
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
