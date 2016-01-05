<?php

$biomaterial = $variables['node']->biomaterial;
$values = $variables['feature_ids'];

  if (sizeof($values['feature_id']) > 0) { 
    $element = 0;
    $num_per_page = 25;
  
    $columns = array('feature_id');
    $options = array(
      'pager' => array(
        'limit' => $num_per_page,
        'element' => $element
      ),
      'order_by' => array('name' => 'ASC'),
    ); 
    $results = chado_select_record('feature', $columns, $values, $options);
  
    $features = array();
    foreach ($results as $result) { 
      $values = array('feature_id' => $result->feature_id);
      $options = array(
        'include_fk' => array(
          'type_id' => 1
        )
      );
      $features[] = chado_generate_var('feature', $values, $options);
    }
  
    if (count($features) > 0) { ?>
      <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">Features taken from this biomaterial are displayed in the following browser.</div> <?php
   
      $headers = array('Feature Name' , 'Unique Name', 'Type');
      $rows = array();
    
      foreach($features as $feature) { 
        $fname = $feature->name;
        if (property_exists($feature, 'nid')) { 
          $fname = l($fname, "node/$feature->nid", array('attributes' => array('target' => '_blank')));
        }
        $rows[] = array(
          $fname,
          $feature->uniquename,
          $feature->type_id->name
        );
      }
   
      $table = array(
        'header' =>  $headers,
        'rows' => $rows,
        'attributes' => array(
          'id' => 'tripal_biomaterial-table-features',
          'class' => 'tripal-data-table'
        ),
        'sticky' => FALSE,
        'caption' => '',
        'colgroups' => array(),
        'empty' => '',
      );
      
      print theme_table($table);
    
      $pager = array(
        'tags' => array(),
        'element' => $element, 
        'parameters' => array(
          'block' => 'biomaterial_feature_browser'
        ),
        'quantity' => $num_per_page,
      );
      print theme_pager($pager);
    }
}
