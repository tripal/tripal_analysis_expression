<?php

function tripal_analysis_expression_preprocess_tripal_analysis_expression_base(&$variables) {

$analysis = $variables['node']->analysis;
$analysis = chado_expand_var($analysis, 'table', 'analysisprop', array('return_array' => 1));
$properties = $analysis->analysisprop;

if (count($properties) > 0) {
  foreach($properties as $property) {
    switch($property->type_id->name) {
      case 'analysis_expression_organism_id':
        $variables['organism'] = chado_select_record('organism', 
          array('genus', 'species', 'common_name'), 
          array('organism_id' => $property->value));
        $variables['organism']['nid'] = chado_get_nid_from_id('organism', $property->value, 'chado_organism');   
        break;
    }
  }
}

}

function tripal_analysis_expression_preprocess_tripal_analysis_expression_protocols(&$variables) {

$analysis = $variables['node']->analysis;
$analysis = chado_expand_var($analysis, 'table', 'analysisprop', array('return_array' => 1));
$properties = $analysis->analysisprop;

if (count($properties) > 0) {
  foreach($properties as $property) {
  //  switch($property->type_id->name) {
      //case 'analysis_expression_organism_id':
      if ($property->type_id->name == 'analysis_expression_organism_id') {
        $variables['organism'] = chado_select_record('organism', 
          array('genus', 'species', 'common_name'), 
          array('organism_id' => $property->value));
        $variables['organism']['nid'] = chado_get_nid_from_id('organism', $property->value, 'chado_organism');   
        } //break;

      if ($property->type_id->name == 'analysis_expression_biosourceprovider_id' && $property->value) {
      //case 'analysis_expression_biosourceprovider_id':
       // if ($property->value) {
        $variables['biosourceprovider'] = chado_select_record('contact', 
          array('name'), 
          array('contact_id' => $property->value));
        $variables['biosourceprovider']['nid'] = chado_get_nid_from_id('contact', $property->value, 'chado_contact');   
        } //break;
      
      if ($property->type_id->name == 'analysis_expression_assaydate') {
      //case 'analysis_expression_assaydate':
        $variables['assaydate'] = $property->value;   
       } // break;

  //    case 'analysis_expression_acquisitiondate':
      if ($property->type_id->name == 'analysis_expression_acquisitiondate') {
        $variables['acquisitiondate'] = $property->value;   
        }//break;
      
      if ($property->type_id->name == 'analysis_expression_quantificationdate') {
      //case 'analysis_expression_quantificationdate':
        $variables['quantificationdate'] = $property->value;   
        } //break;

      //case 'analysis_expression_assay_protocol_id':
      if ($property->type_id->name == 'analysis_expression_assay_protocol_id' and $property->value) {
        //if ($property->value) { 
        $variables['assay_protocol'] = chado_select_record('protocol', 
          array('name'), 
          array('protocol_id' => $property->value));
        $variables['assay_protocol']['nid'] = chado_get_nid_from_id('protocol', $property->value, 'chado_protocol');   
      }// break;
      //else {
      elseif ($property->type_id->name == 'analysis_expression_assay_protocol_id' and !$property->value) {
        $variables['assay_protocol'] = '';
      }

      if ($property->type_id->name == 'analysis_expression_acquisition_protocol_id' and $property->value) {  
      //case 'analysis_expression_acquisition_protocol_id': 
        //if ($property->value) {
        $variables['acquisition_protocol'] = chado_select_record('protocol', 
          array('name'), 
          array('protocol_id' => $property->value));
        $variables['acquisition_protocol']['nid'] = chado_get_nid_from_id('protocol', $property->value, 'chado_protocol');   
      } //break;
      elseif ($property->type_id->name == 'analysis_expression_acquisition_protocol_id' and !$property->value) {  
        $variables['acquisition_protocol'] = '';
      }

      if ($property->type_id->name == 'analysis_expression_quantification_protocol_id' and $property->value) {
      //case 'analysis_expression_quantification_protocol_id':
        //if ($property->value) {
        $variables['quantification_protocol'] = chado_select_record('protocol', 
          array('name'), 
          array('protocol_id' => $property->value));
        $variables['quantification_protocol']['nid'] = chado_get_nid_from_id('protocol', $property->value, 'chado_protocol');   
      } //break;
      //else {
      elseif ($property->type_id->name == 'analysis_expression_quantification_protocol_id' and !$property->value) {
        $variables['quantification_protocol'] = '';
      }

    //} 
  } 
}

}

function tripal_analysis_expression_preprocess_tripal_feature_expression(&$variables) {

  $hide_differential_expression = variable_get('chado_feature_expression_hide_differential_expression');
  $hide_biomaterial_labels = variable_get('chado_feature_expression_hide_biomaterial_labels');
  $limit_label_length = trim(variable_get('chado_feature_expression_limit_label_length'));
  $expression_display = variable_get('chado_feature_expression_expression_display');
  $biomaterial_display_width = variable_get('chado_feature_expression_biomaterial_display_width');
  $min_num_biomaterials = variable_get('chado_feature_expression_min_num_biomaterials');
  $node = $variables['node'];
 
  $feature_id = chado_get_id_from_nid('feature', $node->nid);

  $sql = "SELECT ER.signal, B.name, B.biomaterial_id FROM {elementresult} ER
           INNER JOIN {element} E ON E.element_id = ER.element_id
           INNER JOIN {feature} F ON F.feature_id = E.feature_id
           INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
           INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
           INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
           INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
           INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
         WHERE
           F.feature_id = :feature_id";

  $expressions = chado_query($sql, array(":feature_id" => $feature_id));

  $json_exp = "[";
  foreach($expressions as $exp) {
 
    $desc = chado_get_property(
      array('table' => 'biomaterial', 'id' => $exp->biomaterial_id),
      array('type_name' => 'description', 'cv_name' => 'biomaterial_property')
    );

    if($desc) {
      $description = $desc->value;
    }
    else {
      $description = '';
    }

    $json_exp .= '{"name": "' . $exp->name . '", "intensity": "' . $exp->signal . '", "description": "' . $description . '"},';
  }
  $json_exp .= "]";
  if($json_exp == "[]") {
    $json_exp = "";
  }

  if($expressions->rowCount() < $min_num_biomaterials) {
    $json_exp = "";
  }

  if($hide_differential_expression == 1) {
    $json_exp = "";
  }

  $variables['json_exp'] = $json_exp;
  
  $variables['limit_label_length'] = $limit_label_length;
  $variables['hide_differential_expression'] = $hide_differential_expression;
  $variables['hide_biomaterial_labels'] = $hide_biomaterial_labels;
  $variables['expression_display'] = $expression_display;
  $variables['biomaterial_display_width'] = $biomaterial_display_width;
  $variables['min_num_biomaterials'] = $min_num_biomaterials;

  drupal_add_css(drupal_get_path('module','tripal_analysis_expression') . '/theme/css/expression.css');
 // $variables['styles'] = drupal_get_css();

  drupal_add_js(drupal_get_path('module','tripal_analysis_expression') . '/theme/js/expression.js');
 // $variables['scripts'] = drupal_get_js();
  //$scripts = drupal_get_js();


}

function tripal_analysis_expression_preprocess_tripal_biomaterial_feature_browser(&$variables) {

  $biomaterial = $variables['node']->biomaterial;
  $biomaterial_id = $biomaterial->biomaterial_id;

  $sql = 'SELECT F.feature_id FROM {feature} F
           INNER JOIN {element} E ON F.feature_id = E.feature_id
           INNER JOIN {elementresult} ER ON E.element_id = ER.element_id
           INNER JOIN {quantification} Q ON ER.quantification_id = Q.quantification_id
           INNER JOIN {acquisition} AC ON Q.acquisition_id = AC.acquisition_id
           INNER JOIN {assay} AY ON AC.assay_id = AY.assay_id
           INNER JOIN {assay_biomaterial} AB ON AY.assay_id = AB.assay_id
           INNER JOIN {biomaterial} B ON AB.biomaterial_id = B.biomaterial_id
         WHERE
           B.biomaterial_id = :biomaterial_id';

  $feature_ids = chado_query($sql, array(":biomaterial_id" => $biomaterial_id)); 
 
//  $ids = array();
  if ($feature_ids->rowCount() > 0)  {  
    foreach ($feature_ids as $feature_id) { 
      $ids[] = $feature_id->feature_id;
    }
    $match['feature_id'] = $ids;
    $variables['feature_ids'] = $match;
  }
  else { 
    $match['feature_id'] = [];
    $variables['feature_ids'] = $match;
  } 

}

function tripal_analysis_expression_preprocess_tripal_organism_biomaterial(&$variables) {

  $organism = $variables['node']->organism;
  $organism_id = $organism->organism_id;

  $sql = 'SELECT B.biomaterial_id FROM {biomaterial} B
           INNER JOIN {organism} O ON B.taxon_id = O.organism_id
         WHERE
           O.organism_id = :organism_id';

  $biomaterial_ids = chado_query($sql, array(":organism_id" => $organism_id));

  if ($biomaterial_ids->rowCount() > 0) {
    foreach ($biomaterial_ids as $biomaterial_id) {
      $ids[] = $biomaterial_id->biomaterial_id;
    }
    $match['biomaterial_id'] = $ids;
    $variables['biomaterial_ids'] = $match;
  }
  else {
    $match['biomaterial_id'] = [];
    $variables['biomaterial_ids'] = $match;
  }
}





















