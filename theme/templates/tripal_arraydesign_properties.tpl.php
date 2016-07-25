<?php
  // This line is added so that no errors show in the TOC admin page. 
  if (property_exists($variables['node'],'arraydesign')) { 
    $arraydesign = $variables['node']->arraydesign;
    $arraydesign = chado_expand_var($arraydesign, 'table', 'arraydesignprop', array('return_array' => 1));
    $properties = $arraydesign->arraydesignprop;

    // Check for properties.  
    if (count($properties) > 0) { ?>
      <div class="tripal_arraydesign-data-block-desc tripal-data-block-desc">Additional information about this array design:</div><?php
  
      $header = array('Property Name', 'Value');
  
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
          'id' => 'tripal_arraydesign-table-properties',
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
