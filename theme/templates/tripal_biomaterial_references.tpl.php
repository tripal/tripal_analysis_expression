<?php

$biomaterial = $variables['node']->biomaterial;
$references = array();

if ($biomaterial->dbxref_id) {
  $biomaterial->dbxref_id->is_primary = 1;
  $references[] = $biomaterial->dbxref_id;
}

$options = array('return_array' => 1);
$biomaterial = chado_expand_var($biomaterial, 'table', 'biomaterial_dbxref', $options);
$biomaterial_dbxrefs = $biomaterial->biomaterial_dbxref;
if (count($biomaterial_dbxrefs) > 0) {
  foreach ($biomaterial_dbxrefs as $biomaterial_dbxref) {
    $references[] = $biomaterial_dbxref->dbxref_id;
  }
}

if (count($references) > 0) { ?>
  <div class="tripal_biomaterial-data-block-desc tripal-data-block-desc">External references for this biomaterial:</div><?php

  $headers = array('Database', 'Accession');
  $rows = array();

  foreach ($references as $dbxref) {
    $dbname = $dbxref->db_id->name;
    if ($dbxref->db_id->url) {
      $dbname = l($dbname, $dbxref->db_id->url, array('attributes' => array('target' => '_blank')));
    }

    $accession = $dbxref->accession;
    if ($dbxref->db_id->urlprefix) {
      $accession = l($accession, $dbxref->db_id->urlprefix . $dbxref->accession, array('attributes' => array('target' => "_blank")));
    }
    if (property_exists($dbxref, 'is_primary')) {
      $accession .= " <i>(primary cross-reference)</i>";
    }
    $rows[] = array(
      $dbname,
      $accession
    );
  }
    
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_biomaterial-table-references',
      'class' => 'tripal-data-table',
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  print theme_table($table);
} ?>
   
