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

function tripal_analysis_expression_preprocess_tripal_analysis_biomaterial(&$variables) {

  $analysis = $variables['node']->analysis;
  $analysis = chado_expand_var($analysis, 'table', 'analysisprop', array('return_array' => 1));
  $analysis_id = $analysis->analysis_id;

  $sql = "SELECT B.biomaterial_id FROM {biomaterial} B
           INNER JOIN {assay_biomaterial} AB ON B.biomaterial_id = AB.biomaterial_id
           INNER JOIN {assay} AY ON AB.assay_id = AY.assay_id
           INNER JOIN {acquisition} AQ ON AY.assay_id = AQ.assay_id
           INNER JOIN {quantification} Q ON AQ.acquisition_id = Q.acquisition_id
           INNER JOIN {analysis} A ON Q.analysis_id = A.analysis_id
           INNER JOIN {analysisprop} AP ON A.analysis_id = AP.analysis_id
           INNER JOIN {cvterm} CVT ON AP.type_id = CVT.cvterm_id
         WHERE
           CVT.name = :type_name
         AND 
           A.analysis_id = :analysis_id";

  $biomaterial_ids = chado_query($sql, array(":type_name" => "analysis_expression_associated_analysis_id", ":analysis_id" => $analysis_id));

  if ($biomaterial_ids->rowCount() > 0) {
    foreach ($biomaterial_ids as $biomaterial_id) {
      $ids[] = $biomaterial_id->biomaterial_id;
    }
    $variables['biomaterial_ids'] = $ids;
  }
  else {
    $variables['biomaterial_ids'] = [];
  }

}

function tripal_analysis_expression_preprocess_tripal_biomaterial_base(&$variables) {

  $biomaterial_ontologies = variable_get('chado_biomaterial_ontologies_to_display');
//  $biomaterial_ontologies = "sequence";

  $ontology_names = preg_split('/\s+/', $biomaterial_ontologies);

  $biomaterial = $variables['node']->biomaterial;
  $biomaterial_id = $biomaterial->biomaterial_id;

  $ontology_array = array();

  foreach($ontology_names as $ontology_name) {

    $sql = 'SELECT CVT.name FROM {cvterm} CVT
             INNER JOIN {biomaterialprop} BP ON CVT.cvterm_id = BP.type_id
             INNER JOIN {biomaterial} B ON BP.biomaterial_id = B.biomaterial_id
             INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
           WHERE
             BP.biomaterial_id = :biomaterial_id
           AND
             CV.name = :cv_name';
  
    $ontology_terms = chado_query($sql, array(":biomaterial_id" => $biomaterial_id, ":cv_name" => $ontology_name));
  
    $terms = array();
  
    foreach($ontology_terms as $ontology_term) {
      $terms[] = $ontology_term->name;
    }

    $ontology_array[$ontology_name] = $terms;

  }

  // Check to see if there is an analysis associated with this biomaterial.
  $sql = "SELECT DISTINCT A.name, A.analysis_id from {analysis} A 
           INNER JOIN {quantification} Q ON A.analysis_id = Q.analysis_id
           INNER JOIN {acquisition} AC ON Q.acquisition_id = AC.acquisition_id
           INNER JOIN {assay} AY ON AC.assay_id = AY.assay_id
           INNER JOIN {assay_biomaterial} AB ON AY.assay_id = AB.assay_id
         WHERE AB.biomaterial_id = :biomaterial_id LIMIT 1";

  // This assumes there is only one analysis associated with this biomterial using the above tables.
  $analysis = chado_query($sql, array(":biomaterial_id" => $biomaterial_id))->fetchObject();

  $variables['analysis_name'] = ($analysis and property_exists($analysis, "name")) ? $analysis->name: '';
  $variables['analysis_nid'] = ($analysis and property_exists($analysis, "analysis_id")) ? chado_get_nid_from_id('analysis', $analysis->analysis_id, 'chado_analysis'): '';
  $variables['biomaterial_ontology_terms'] = $ontology_array;

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

  $hide_expression = variable_get('chado_feature_expression_hide_expression');
  $hide_biomaterial_labels = variable_get('chado_feature_expression_hide_biomaterial_labels');
  $limit_label_length = trim(variable_get('chado_feature_expression_limit_label_length'));
  $expression_display = variable_get('chado_feature_expression_expression_display');
  $biomaterial_display_width = variable_get('chado_feature_expression_biomaterial_display_width');
  $min_num_biomaterials = variable_get('chado_feature_expression_min_num_biomaterials');
  $node = $variables['node'];

  $has_exp = FALSE;
  $first_sig = 1;

  $feature_id = chado_get_id_from_nid('feature', $node->nid);

  $sql = "SELECT ER.signal, B.name, B.biomaterial_id, B.description FROM {elementresult} ER
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
    if($first_sig == 1) {
      $last_signal = $exp->signal;
      $first_sig = 0;
    }

    if ($exp->signal != $last_signal and $exp->signal != 0) {
      $has_exp = TRUE;
    }
    $last_signal = $exp->signal;

    $tis = chado_get_property(
      array('table' => 'biomaterial', 'id' => $exp->biomaterial_id),
      array('type_name' => 'tissue', 'cv_name' => 'biomaterial_property')
    );

    if($tis) {
      $tissue = $tis->value;
    }
    else {
      $tissue = '';
    }


    $treat = chado_get_property(
      array('table' => 'biomaterial', 'id' => $exp->biomaterial_id),
      array('type_name' => 'treatment', 'cv_name' => 'biomaterial_property')
    );

    if($treat) {
      $treatment = $treat->value;
    }
    else {
      $treatment = '';
    }

    $node_present = chado_get_nid_from_id('biomaterial', $exp->biomaterial_id);


    $json_exp .= '{"name": "' . $exp->name . '", "intensity": "' . $exp->signal . '", "description": "' . $exp->description . '", "tissue": "' . $tissue . '", "treatment": "' . $treatment . '", "node": "' . $node_present . '"},';
  }
  $json_exp .= "]";
  if($json_exp == "[]") {
    $json_exp = "";
  }

  if($expressions->rowCount() < $min_num_biomaterials) {
    $json_exp = "";
  }

  if($hide_expression == 1) {
    $json_exp = "";
  }


  $variables['has_exp'] = $has_exp;
  $variables['json_exp'] = $json_exp;

  $variables['limit_label_length'] = $limit_label_length;
  $variables['hide_expression'] = $hide_expression;
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
    $variables['biomaterial_ids'] = $ids;
  }
  else {
    $variables['biomaterial_ids'] = [];
  }
}





















