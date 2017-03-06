<?php
  // This line is added so that no errors show in the TOC admin page. 
  if (property_exists($variables['node'],'biomaterial')) { 
    $biomaterial = $variables['node']->biomaterial;
    $biomaterial = chado_expand_var($biomaterial, 'table', 'biomaterialprop', array('return_array' => 1));
    $properties = $biomaterial->biomaterialprop;

    // Check for properties.  
    if (count($properties) > 0) { ?>
      <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">Additional information about this biomaterial:</div><?php
  
      $headers = array('Property Name', 'Value');
  
      $rows = array();
      foreach ($properties as $property) {
        $rows[] = array(
          ucfirst(preg_replace('/_/', ' ', $property->type_id->name)),
          $property->value
        );
      }
  
      $table = array(
        'header' => $headers,
        'rows' => $rows,
        'attributes' => array(
          'id' => 'tripal_biomaterial-table-properties',
          'class' => 'tripal-data-table'
        ),
        'sticky' => FALSE,
        'caption' => '',
        'colgroups' => array(),
        'empty' => '',
      );
  
      print theme_table($table);
    }
  } 
?>
