<?php
  // This conditional is added to prevent errors in the array design TOC admin page.
  if (property_exists($variables['node'],'arraydesign')) {  
    $arraydesign = $variables['node']->arraydesign;
    $arraydesign = chado_expand_var($arraydesign, 'field', 'arraydesign.description'); 
    ?>
    
    <div class="tripal_arraydesign-data-block-desc tripal-data-block-desc"></div>
    
  <?php
    
    $headers = array();
    $rows = array();
    
    // The array design name 
    $rows[] = array(
      array(
        'data' => 'Array Design',
        'header' => TRUE,
        'width' => '20%', 
      ),
      '<i>' . $arraydesign->name . '</i>'
    );
   
    // The name of the manufacturer. The manufacturer is an entry in the contact table. 
    $rows[] = array(
      array(
        'data' => 'Manufacturer',
        'header' => TRUE,
        'width' => '20%',
      ),
      '<i>' . $arraydesign->manufacturer_id->name . '</i>'
    );
   
    // The version of the array design. 
    if ($arraydesign->version) {
      $rows[] = array(
        array(
          'data' => 'Version',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i>' . $arraydesign->version . '</i>'
      );
    }
   
    //The array dimensions, number of array columns and number of array rows are all displayed in a single field. 
    if ($arraydesign->array_dimensions or $arraydesign->num_array_columns or $arraydesign->num_array_rows) {
      $afield = '';
      if($arraydesign->array_dimensions) {
        $afield = $afield . $arraydesign->array_dimensions;
      }
      if($arraydesign->array_dimensions and $arraydesign->num_array_columns and $arraydesign->num_array_rows) {
        $afield = $afield . ' - ';
      }
      if($arraydesign->num_array_columns) {
        $afield = $afield . $arraydesign->num_array_columns . ' column(s)';
      }
      if($arraydesign->num_array_columns and $arraydesign->num_array_rows) {
        $afield = $afield . ', ';
      }
      if($arraydesign->num_array_rows) {
        $afield = $afield . $arraydesign->num_array_rows . ' row(s)';
      }
      $rows[] = array(
        array(
          'data' => 'Array',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i>' . $afield . '</i>'
      );
    }
  
    // The element dimensions and the number of elements are displayed in a single field.
    if ($arraydesign->element_dimensions or $arraydesign->num_of_elements) {
      $efield = '';
      if ($arraydesign->element_dimensions) {
        $efield = $efield . $arraydesign->element_dimensions;
      }
      if ($arraydesign->element_dimensions and $arraydesign->num_of_elements) {
        $efield = $efield . ' - ';
      }
      if ($arraydesign->num_of_elements) {
        $efield = $efield . $arraydesign->num_of_elements . ' element(s)';
      }
      $rows[] = array(
        array(
          'data' => 'Element',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i>' . $efield . '</i>'
      );
    
    }
   
    // The number of grid columns and rows are displayed in the same field. 
    if ($arraydesign->num_grid_columns or $arraydesign->num_grid_rows) {
      $gfield = '';
      if ($arraydesign->num_grid_columns) {
        $gfield = $gfield . $arraydesign->num_grid_columns . ' column(s)';
      }
      if ($arraydesign->num_grid_columns and $arraydesign->num_grid_rows) {
        $gfield = $gfield . ', ';
      }
      if ($arraydesign->num_grid_rows) {
        $gfield = $gfield . $arraydesign->num_grid_rows . ' row(s)';
      }
      $rows[] = array(
        array(
          'data' => 'Grid',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i>' . $gfield . '</i>'
      );
    }
   
    // The numbe of sub columns and rows are displayed in the same field. 
    if ($arraydesign->num_sub_columns or $arraydesign->num_sub_rows) {
      $sfield = '';
      if ($arraydesign->num_sub_columns) {
        $sfield = $sfield . $arraydesign->num_sub_columns . ' column(s)';
      }
      if ($arraydesign->num_sub_columns and $arraydesign->num_sub_rows) {
        $sfield = $sfield . ', ';
      }
      if ($arraydesign->num_sub_rows) {
        $sfield = $sfield . $arraydesign->num_sub_rows . ' row(s)';
      }
      $rows[] = array(
        array(
          'data' => 'Sub - (what is this?)',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i>' . $sfield . '</i>'
      );
    }
   
    // The platform type. 
    if ($arraydesign->platformtype_id) {
      $rows[] = array(
        array(
          'data' => 'Platform',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i>' . $arraydesign->platformtype_id->name . '</i>'
      );
    }
   
    // The substrate type. 
    if ($arraydesign->substratetype_id) {
      $rows[] = array(
        array(
          'data' => 'Substrate',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i>' . $arraydesign->substratetype_id->name . '</i>'
      );
    }
   
    // The name of the protocol. 
    if ($arraydesign->protocol_id) { //may need to check for names
      $rows[] = array(
        array(
          'data' => 'Protocol',
          'header' => TRUE,
          'width' => '20%',
        ),
        '<i><a href=/protocol/' . $arraydesign->protocol_id->name . '>' . $arraydesign->protocol_id->name . '</a></i>'
      );
    }
   
    // allow site admins to see the arraydesign ID
    if (user_access('view ids')) {
      // Arraydesign ID
      $rows[] = array(
        array(
          'data' => 'Arraydesign ID',
          'header' => TRUE,
          'class' => 'tripal-site-admin-only-table-row',
        ),
        array(
          'data' => $arraydesign->arraydesign_id,
          'class' => 'tripal-site-admin-only-table-row',
        ),
      );
    }
 
    // Generate the table of data provided above. 
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_arraydesign-table-base',
        'class' => 'tripal-arraydesign-data-table tripal-data-table',
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    );
   
    // Print the table and the description.
    print theme_table($table); ?> 
    <div style="text-align: justify"><?php print $arraydesign->description?></div>
  <?php 
  } 
?>
