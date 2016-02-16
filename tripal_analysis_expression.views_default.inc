<?php
/**
 * @file
 * Analysis: Expression default views.
 */
 
/** 
 * Impelments hook_views_default_views().
 *
 * This function informs tripal of the default views of the 
 * Analysis: Expression module. An administrator view and an anonymous
 * user view are defined for the four content types defined by this 
 * module: analysis: expression, arraydesign, biomaterial, and protocol.  
 * 
 * @ingroup tripal_analyis_expression
 */
function tripal_analysis_expression_views_default_views() { 
  $views = array();
  
  $view = tripal_arraydesign_defaultvalue_user_arraydesigns();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;
 
  $view = tripal_protocol_defaultvalue_user_protocols();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;
 
  $view = tripal_biomaterial_defaultvalue_user_biomaterials();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_biomaterial_defaultvalue_user_ontologies();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;
 
  $view = tripal_analysis_expression_defaultvalue_user_analysis_expressions();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;
  
  $view = tripal_analysis_expression_admin_defaultvalue_analysis_expressions();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_arraydesign_admin_defaultview_arraydesigns();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;
  
  $view = tripal_protocol_admin_defaultview_protocols();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_biomaterial_admin_defaultview_biomaterials();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;
 
  return $views;
} 

function tripal_biomaterial_defaultvalue_user_ontologies() {

$view = new view();
$view->name = 'biomaterial_ontology_terms';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'biomaterial';
$view->human_name = 'Biomaterial Ontology Terms';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Biomaterial Ontology Terms';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'access chado_biomaterial content';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
/* Relationship: Biomaterial: Biomaterial => Node */
$handler->display->display_options['relationships']['biomaterial_id']['id'] = 'biomaterial_id';
$handler->display->display_options['relationships']['biomaterial_id']['table'] = 'chado_biomaterial';
$handler->display->display_options['relationships']['biomaterial_id']['field'] = 'biomaterial_id';
$handler->display->display_options['relationships']['biomaterial_id']['required'] = TRUE;
/* Relationship: Chado Biomaterial: biomaterial_id => Biomaterialprop (biomaterial_id) */
$handler->display->display_options['relationships']['biomaterial_id_to_biomaterialprop']['id'] = 'biomaterial_id_to_biomaterialprop';
$handler->display->display_options['relationships']['biomaterial_id_to_biomaterialprop']['table'] = 'biomaterial';
$handler->display->display_options['relationships']['biomaterial_id_to_biomaterialprop']['field'] = 'biomaterial_id_to_biomaterialprop';
$handler->display->display_options['relationships']['biomaterial_id_to_biomaterialprop']['required'] = TRUE;
/* Relationship: Chado Biomaterialprop: type_id => Cvterm (cvterm_id) */
$handler->display->display_options['relationships']['type_id_to_cvterm']['id'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['table'] = 'biomaterialprop';
$handler->display->display_options['relationships']['type_id_to_cvterm']['field'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['relationship'] = 'biomaterial_id_to_biomaterialprop';
$handler->display->display_options['relationships']['type_id_to_cvterm']['required'] = TRUE;
/* Relationship: Chado Cvterm: cv_id => Cv (cv_id) */
$handler->display->display_options['relationships']['cv_id_to_cv']['id'] = 'cv_id_to_cv';
$handler->display->display_options['relationships']['cv_id_to_cv']['table'] = 'cvterm';
$handler->display->display_options['relationships']['cv_id_to_cv']['field'] = 'cv_id_to_cv';
$handler->display->display_options['relationships']['cv_id_to_cv']['relationship'] = 'biomaterial_id_to_biomaterialprop';
$handler->display->display_options['relationships']['cv_id_to_cv']['required'] = TRUE;
/* Relationship: Chado Biomaterial: taxon_id => Organism (organism_id) */
$handler->display->display_options['relationships']['taxon_id_to_organism']['id'] = 'taxon_id_to_organism';
$handler->display->display_options['relationships']['taxon_id_to_organism']['table'] = 'biomaterial';
$handler->display->display_options['relationships']['taxon_id_to_organism']['field'] = 'taxon_id_to_organism';
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['relationship'] = 'biomaterial_id';
$handler->display->display_options['fields']['path']['label'] = '';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
$handler->display->display_options['fields']['path']['element_label_colon'] = FALSE;
/* Field: Chado Biomaterial: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'biomaterial';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['label'] = 'Biomaterial';
$handler->display->display_options['fields']['name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['path'] = '[path]';
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: Chado Organism: Common Name */
$handler->display->display_options['fields']['common_name']['id'] = 'common_name';
$handler->display->display_options['fields']['common_name']['table'] = 'organism';
$handler->display->display_options['fields']['common_name']['field'] = 'common_name';
$handler->display->display_options['fields']['common_name']['label'] = 'Organism';
$handler->display->display_options['fields']['common_name']['element_label_colon'] = FALSE;
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'cvterm';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['relationship'] = 'biomaterial_id_to_biomaterialprop';
$handler->display->display_options['fields']['name_1']['label'] = 'Ontology Term';
/* Field: Chado Cv: Name */
$handler->display->display_options['fields']['name_2']['id'] = 'name_2';
$handler->display->display_options['fields']['name_2']['table'] = 'cv';
$handler->display->display_options['fields']['name_2']['field'] = 'name';
$handler->display->display_options['fields']['name_2']['relationship'] = 'type_id_to_cvterm';
$handler->display->display_options['fields']['name_2']['label'] = 'Ontology';
/* Contextual filter: Chado Cv: Name */
$handler->display->display_options['arguments']['name_1']['id'] = 'name_1';
$handler->display->display_options['arguments']['name_1']['table'] = 'cv';
$handler->display->display_options['arguments']['name_1']['field'] = 'name';
$handler->display->display_options['arguments']['name_1']['relationship'] = 'type_id_to_cvterm';
$handler->display->display_options['arguments']['name_1']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['name_1']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['name_1']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['name_1']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['name_1']['limit'] = '0';
/* Contextual filter: Chado Cvterm: Name */
$handler->display->display_options['arguments']['name']['id'] = 'name';
$handler->display->display_options['arguments']['name']['table'] = 'cvterm';
$handler->display->display_options['arguments']['name']['field'] = 'name';
$handler->display->display_options['arguments']['name']['relationship'] = 'biomaterial_id_to_biomaterialprop';
$handler->display->display_options['arguments']['name']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['name']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['name']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['name']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['name']['limit'] = '0';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'biomaterial-ontology-terms';

return $view;
}


/**
 * This function contains the default view for the analysis: expression
 * content type for anonymous users. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_analysis_expression_defaultvalue_user_analysis_expressions() {

$view = new view();
$view->name = 'tripal_analysis_expression_user_analysis_expressions';
$view->description = 'Allows users to search array designs.';
$view->tag = 'default';
$view->base_table = 'analysis';
$view->human_name = 'Analysis: Expressions User Search';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Analysis: Expressions';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer tripal analysis expression';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
/* Relationship: Chado Analysis: analysis_id => Analysisprop (analysis_id) */
$handler->display->display_options['relationships']['analysis_id_to_analysisprop']['id'] = 'analysis_id_to_analysisprop';
$handler->display->display_options['relationships']['analysis_id_to_analysisprop']['table'] = 'analysis';
$handler->display->display_options['relationships']['analysis_id_to_analysisprop']['field'] = 'analysis_id_to_analysisprop';
/* Relationship: Chado Analysisprop: type_id => Cvterm (cvterm_id) */
$handler->display->display_options['relationships']['type_id_to_cvterm']['id'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['table'] = 'analysisprop';
$handler->display->display_options['relationships']['type_id_to_cvterm']['field'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['relationship'] = 'analysis_id_to_analysisprop';
/* Relationship: Analysis: Analysis => Node */
$handler->display->display_options['relationships']['analysis_id']['id'] = 'analysis_id';
$handler->display->display_options['relationships']['analysis_id']['table'] = 'chado_analysis';
$handler->display->display_options['relationships']['analysis_id']['field'] = 'analysis_id';
$handler->display->display_options['relationships']['analysis_id']['required'] = TRUE;
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['relationship'] = 'analysis_id';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Chado Analysis: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'analysis';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['path'] = '[path]';
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: Chado Analysis: Program */
$handler->display->display_options['fields']['program']['id'] = 'program';
$handler->display->display_options['fields']['program']['table'] = 'analysis';
$handler->display->display_options['fields']['program']['field'] = 'program';
/* Field: Chado Analysis: Sourcename */
$handler->display->display_options['fields']['sourcename']['id'] = 'sourcename';
$handler->display->display_options['fields']['sourcename']['table'] = 'analysis';
$handler->display->display_options['fields']['sourcename']['field'] = 'sourcename';
$handler->display->display_options['fields']['sourcename']['label'] = 'Source';
/* Filter criterion: Chado Cvterm: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'cvterm';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['relationship'] = 'analysis_id_to_analysisprop';
$handler->display->display_options['filters']['name']['value'] = 'analysis_expression_organism_id';
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Chado Analysis: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'analysis';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['operator'] = 'word';
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name Contains';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';
/* Filter criterion: Chado Analysis: Program */
$handler->display->display_options['filters']['program']['id'] = 'program';
$handler->display->display_options['filters']['program']['table'] = 'analysis';
$handler->display->display_options['filters']['program']['field'] = 'program';
$handler->display->display_options['filters']['program']['operator'] = 'word';
$handler->display->display_options['filters']['program']['exposed'] = TRUE;
$handler->display->display_options['filters']['program']['expose']['operator_id'] = 'program_op';
$handler->display->display_options['filters']['program']['expose']['label'] = 'Program Name Contains';
$handler->display->display_options['filters']['program']['expose']['operator'] = 'program_op';
$handler->display->display_options['filters']['program']['expose']['identifier'] = 'program';
$handler->display->display_options['filters']['program']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['program']['group_info']['label'] = 'Program';
$handler->display->display_options['filters']['program']['group_info']['identifier'] = 'program';
$handler->display->display_options['filters']['program']['group_info']['remember'] = FALSE;
$handler->display->display_options['filters']['program']['group_info']['group_items'] = array(
  1 => array(),
  2 => array(),
  3 => array(),
);
/* Filter criterion: Chado Analysis: Sourcename */
$handler->display->display_options['filters']['sourcename']['id'] = 'sourcename';
$handler->display->display_options['filters']['sourcename']['table'] = 'analysis';
$handler->display->display_options['filters']['sourcename']['field'] = 'sourcename';
$handler->display->display_options['filters']['sourcename']['operator'] = 'word';
$handler->display->display_options['filters']['sourcename']['exposed'] = TRUE;
$handler->display->display_options['filters']['sourcename']['expose']['operator_id'] = 'sourcename_op';
$handler->display->display_options['filters']['sourcename']['expose']['label'] = 'Source Contains';
$handler->display->display_options['filters']['sourcename']['expose']['operator'] = 'sourcename_op';
$handler->display->display_options['filters']['sourcename']['expose']['identifier'] = 'sourcename';
$handler->display->display_options['filters']['sourcename']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['sourcename']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'chado/analysis-expression';
$handler->display->display_options['menu']['title'] = 'Analysis: Expression';
$handler->display->display_options['menu']['description'] = 'A list of expression analyses.';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;
}

/**
 * This function contains the default view for the analysis: expression
 * content type for admins. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_analysis_expression_admin_defaultvalue_analysis_expressions() {

$view = new view();
$view->name = 'tripal_analysis_expression_admin_analysis_expressions';
$view->description = 'DO NOT DISABLE';
$view->tag = 'default';
$view->base_table = 'analysis';
$view->human_name = 'Analysis: Expressions Admin';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Analysis: Expressions';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer tripal analysis expression';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
/* Header: Global: Action Links */
$handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
$handler->display->display_options['header']['action_links_area']['table'] = 'views';
$handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
$handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
$handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
$handler->display->display_options['header']['action_links_area']['link-1'] = array(
  'label-1' => 'Add Analysis: Expression',
  'path-1' => '/node/add/chado-analysis-expression',
);
$handler->display->display_options['header']['action_links_area']['link-2'] = array(
  'label-2' => '',
  'path-2' => '',
);
$handler->display->display_options['header']['action_links_area']['link-3'] = array(
  'label-3' => '',
  'path-3' => '',
);
$handler->display->display_options['header']['action_links_area']['link-4'] = array(
  'label-4' => '',
  'path-4' => '',
);
/* Relationship: Chado Analysis: analysis_id => Analysisprop (analysis_id) */
$handler->display->display_options['relationships']['analysis_id_to_analysisprop']['id'] = 'analysis_id_to_analysisprop';
$handler->display->display_options['relationships']['analysis_id_to_analysisprop']['table'] = 'analysis';
$handler->display->display_options['relationships']['analysis_id_to_analysisprop']['field'] = 'analysis_id_to_analysisprop';
/* Relationship: Chado Analysisprop: type_id => Cvterm (cvterm_id) */
$handler->display->display_options['relationships']['type_id_to_cvterm']['id'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['table'] = 'analysisprop';
$handler->display->display_options['relationships']['type_id_to_cvterm']['field'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['relationship'] = 'analysis_id_to_analysisprop';
/* Relationship: Analysis: Analysis => Node */
$handler->display->display_options['relationships']['analysis_id']['id'] = 'analysis_id';
$handler->display->display_options['relationships']['analysis_id']['table'] = 'chado_analysis';
$handler->display->display_options['relationships']['analysis_id']['field'] = 'analysis_id';
$handler->display->display_options['relationships']['analysis_id']['required'] = TRUE;
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['relationship'] = 'analysis_id';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Chado Analysis: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'analysis';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['path'] = '[path]';
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: Chado Analysis: Program */
$handler->display->display_options['fields']['program']['id'] = 'program';
$handler->display->display_options['fields']['program']['table'] = 'analysis';
$handler->display->display_options['fields']['program']['field'] = 'program';
/* Field: Chado Analysis: Sourcename */
$handler->display->display_options['fields']['sourcename']['id'] = 'sourcename';
$handler->display->display_options['fields']['sourcename']['table'] = 'analysis';
$handler->display->display_options['fields']['sourcename']['field'] = 'sourcename';
$handler->display->display_options['fields']['sourcename']['label'] = 'Source';
/* Field: Content: Edit link */
$handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['relationship'] = 'analysis_id';
$handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
/* Field: Content: Delete link */
$handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['relationship'] = 'analysis_id';
$handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = '';
$handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node] [delete_node]';
$handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
/* Filter criterion: Chado Cvterm: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'cvterm';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['relationship'] = 'analysis_id_to_analysisprop';
$handler->display->display_options['filters']['name']['value'] = 'analysis_expression_organism_id';
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Chado Analysis: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'analysis';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['operator'] = 'word';
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name Contains';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';
/* Filter criterion: Chado Analysis: Program */
$handler->display->display_options['filters']['program']['id'] = 'program';
$handler->display->display_options['filters']['program']['table'] = 'analysis';
$handler->display->display_options['filters']['program']['field'] = 'program';
$handler->display->display_options['filters']['program']['operator'] = 'word';
$handler->display->display_options['filters']['program']['exposed'] = TRUE;
$handler->display->display_options['filters']['program']['expose']['operator_id'] = 'program_op';
$handler->display->display_options['filters']['program']['expose']['label'] = 'Program Name Contains';
$handler->display->display_options['filters']['program']['expose']['operator'] = 'program_op';
$handler->display->display_options['filters']['program']['expose']['identifier'] = 'program';
$handler->display->display_options['filters']['program']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['program']['group_info']['label'] = 'Program';
$handler->display->display_options['filters']['program']['group_info']['identifier'] = 'program';
$handler->display->display_options['filters']['program']['group_info']['remember'] = FALSE;
$handler->display->display_options['filters']['program']['group_info']['group_items'] = array(
  1 => array(),
  2 => array(),
  3 => array(),
);
/* Filter criterion: Chado Analysis: Sourcename */
$handler->display->display_options['filters']['sourcename']['id'] = 'sourcename';
$handler->display->display_options['filters']['sourcename']['table'] = 'analysis';
$handler->display->display_options['filters']['sourcename']['field'] = 'sourcename';
$handler->display->display_options['filters']['sourcename']['operator'] = 'word';
$handler->display->display_options['filters']['sourcename']['exposed'] = TRUE;
$handler->display->display_options['filters']['sourcename']['expose']['operator_id'] = 'sourcename_op';
$handler->display->display_options['filters']['sourcename']['expose']['label'] = 'Source Contains';
$handler->display->display_options['filters']['sourcename']['expose']['operator'] = 'sourcename_op';
$handler->display->display_options['filters']['sourcename']['expose']['identifier'] = 'sourcename';
$handler->display->display_options['filters']['sourcename']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['sourcename']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/tripal/extension/tripal_analysis_expression/content_types/analysis_expression/analysis_expressions';
$handler->display->display_options['menu']['type'] = 'default tab';
$handler->display->display_options['menu']['title'] = 'Analysis: Expression';
$handler->display->display_options['menu']['description'] = 'A list of expression analyses.';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;
}

/**
 * This function contains the default view for the arraydesign
 * content type for anonymous users. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_arraydesign_defaultvalue_user_arraydesigns() {

$view = new view();
$view->name = 'tripal_arraydesign_user_arraydesigns';
$view->description = 'Allows users to search array design data.';
$view->tag = 'tripal search';
$view->base_table = 'arraydesign';
$view->human_name = 'Array design user search';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Array Design';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'access tripal analysis expression';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'name_1' => 'name_1',
  'name' => 'name',
  'name_2' => 'name_2',
  'name_3' => 'name_3',
);
$handler->display->display_options['style_options']['default'] = 'name_1';
$handler->display->display_options['style_options']['info'] = array(
  'name_1' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name_2' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name_3' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'No array designs matched the supplied criteria.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Relationship: Arraydesign: Arraydesign => Node */
$handler->display->display_options['relationships']['arraydesign_id']['id'] = 'arraydesign_id';
$handler->display->display_options['relationships']['arraydesign_id']['table'] = 'chado_arraydesign';
$handler->display->display_options['relationships']['arraydesign_id']['field'] = 'arraydesign_id';
$handler->display->display_options['relationships']['arraydesign_id']['required'] = TRUE;
/* Relationship: Chado Arraydesign: manufacturer_id => Contact */
$handler->display->display_options['relationships']['manufacturer_id_to_contact']['id'] = 'manufacturer_id_to_contact';
$handler->display->display_options['relationships']['manufacturer_id_to_contact']['table'] = 'arraydesign';
$handler->display->display_options['relationships']['manufacturer_id_to_contact']['field'] = 'manufacturer_id_to_contact';
/* Relationship: Chado Arraydesign: platformtype_id => Cvterm */
$handler->display->display_options['relationships']['platformtype_id_to_cvterm']['id'] = 'platformtype_id_to_cvterm';
$handler->display->display_options['relationships']['platformtype_id_to_cvterm']['table'] = 'arraydesign';
$handler->display->display_options['relationships']['platformtype_id_to_cvterm']['field'] = 'platformtype_id_to_cvterm';
/* Relationship: Chado Arraydesign: substratetype_id => Cvterm */
$handler->display->display_options['relationships']['substratetype_id_to_cvterm']['id'] = 'substratetype_id_to_cvterm';
$handler->display->display_options['relationships']['substratetype_id_to_cvterm']['table'] = 'arraydesign';
$handler->display->display_options['relationships']['substratetype_id_to_cvterm']['field'] = 'substratetype_id_to_cvterm';
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Chado Arraydesign: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'arraydesign';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['label'] = 'Array Design';
$handler->display->display_options['fields']['name_1']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name_1']['alter']['path'] = '[path]';
/* Field: Chado Contact: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'contact';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['label'] = 'Manufacturer';
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name_2']['id'] = 'name_2';
$handler->display->display_options['fields']['name_2']['table'] = 'cvterm';
$handler->display->display_options['fields']['name_2']['field'] = 'name';
$handler->display->display_options['fields']['name_2']['relationship'] = 'platformtype_id_to_cvterm';
$handler->display->display_options['fields']['name_2']['label'] = 'Platform';
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name_3']['id'] = 'name_3';
$handler->display->display_options['fields']['name_3']['table'] = 'cvterm';
$handler->display->display_options['fields']['name_3']['field'] = 'name';
$handler->display->display_options['fields']['name_3']['relationship'] = 'substratetype_id_to_cvterm';
$handler->display->display_options['fields']['name_3']['label'] = 'Substrate';
/* Sort criterion: Chado Arraydesign: Name */
$handler->display->display_options['sorts']['name']['id'] = 'name';
$handler->display->display_options['sorts']['name']['table'] = 'arraydesign';
$handler->display->display_options['sorts']['name']['field'] = 'name';
/* Filter criterion: Chado Arraydesign: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'arraydesign';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['operator'] = 'contains';
$handler->display->display_options['filters']['name']['group'] = 1;
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Array Design';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Chado Contact: Name */
$handler->display->display_options['filters']['name_2']['id'] = 'name_2';
$handler->display->display_options['filters']['name_2']['table'] = 'contact';
$handler->display->display_options['filters']['name_2']['field'] = 'name';
$handler->display->display_options['filters']['name_2']['relationship'] = 'manufacturer_id_to_contact';
$handler->display->display_options['filters']['name_2']['operator'] = 'contains';
$handler->display->display_options['filters']['name_2']['group'] = 1;
$handler->display->display_options['filters']['name_2']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_2']['expose']['operator_id'] = 'name_2_op';
$handler->display->display_options['filters']['name_2']['expose']['label'] = 'Manufacturer';
$handler->display->display_options['filters']['name_2']['expose']['operator'] = 'name_2_op';
$handler->display->display_options['filters']['name_2']['expose']['identifier'] = 'name_2';
$handler->display->display_options['filters']['name_2']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_2']['max_length'] = '40';
/* Filter criterion: Chado Cv: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'cv';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['relationship'] = 'platformtype_id_to_cvterm';
$handler->display->display_options['filters']['name_1']['operator'] = 'contains';
$handler->display->display_options['filters']['name_1']['group'] = 1;
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Platform';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';
/* Filter criterion: Chado Cvterm: Name */
$handler->display->display_options['filters']['name_3']['id'] = 'name_3';
$handler->display->display_options['filters']['name_3']['table'] = 'cvterm';
$handler->display->display_options['filters']['name_3']['field'] = 'name';
$handler->display->display_options['filters']['name_3']['relationship'] = 'substratetype_id_to_cvterm';
$handler->display->display_options['filters']['name_3']['operator'] = 'contains';
$handler->display->display_options['filters']['name_3']['group'] = 1;
$handler->display->display_options['filters']['name_3']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_3']['expose']['operator_id'] = 'name_3_op';
$handler->display->display_options['filters']['name_3']['expose']['label'] = 'Substrate';
$handler->display->display_options['filters']['name_3']['expose']['operator'] = 'name_3_op';
$handler->display->display_options['filters']['name_3']['expose']['identifier'] = 'name_3';
$handler->display->display_options['filters']['name_3']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_3']['group_info']['label'] = 'Name';
$handler->display->display_options['filters']['name_3']['group_info']['identifier'] = 'name_3';
$handler->display->display_options['filters']['name_3']['group_info']['remember'] = FALSE;
$handler->display->display_options['filters']['name_3']['group_info']['group_items'] = array(
  1 => array(),
  2 => array(),
  3 => array(),
);
$handler->display->display_options['filters']['name_3']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'chado/arraydesign';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Array Design';
$handler->display->display_options['menu']['description'] = 'An array design.';
$handler->display->display_options['menu']['weight'] = '-10';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;
}

/**
 * This function contains the default view for the arraydesign
 * content type for admin users. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_arraydesign_admin_defaultview_arraydesigns() {
$view = new view();
$view->name = 'tripal_arraydesign_admin_arraydesigns'; //changed this name to tripal_arraydesign_admin_arraydesigns
$view->description = 'DO NOT DISABLE';
$view->tag = 'tripal admin';
$view->base_table = 'arraydesign';
$view->human_name = 'Array Design';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Array Design';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer tripal analysis expression';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'name_1' => 'name_1',
  'name' => 'name',
  'name_2' => 'name_2',
  'name_3' => 'name_3',
);
$handler->display->display_options['style_options']['default'] = 'name_1';
$handler->display->display_options['style_options']['info'] = array(
  'name_1' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name_2' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name_3' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Action Links */
$handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
$handler->display->display_options['header']['action_links_area']['table'] = 'views';
$handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Action Links'; // Copied and modified from organisms
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = array(
    'label-1' => 'Add Array Design',
    'path-1' => 'node/add/chado-arraydesign',
  );  //copied and modified from organism


/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'No array designs matched the supplied criteria.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Relationship: Arraydesign: Arraydesign => Node */
$handler->display->display_options['relationships']['arraydesign_id']['id'] = 'arraydesign_id';
$handler->display->display_options['relationships']['arraydesign_id']['table'] = 'chado_arraydesign';
$handler->display->display_options['relationships']['arraydesign_id']['field'] = 'arraydesign_id';
$handler->display->display_options['relationships']['arraydesign_id']['required'] = TRUE;
/* Relationship: Chado Arraydesign: manufacturer_id => Contact */
$handler->display->display_options['relationships']['manufacturer_id_to_contact']['id'] = 'manufacturer_id_to_contact';
$handler->display->display_options['relationships']['manufacturer_id_to_contact']['table'] = 'arraydesign';
$handler->display->display_options['relationships']['manufacturer_id_to_contact']['field'] = 'manufacturer_id_to_contact';
/* Relationship: Chado Arraydesign: platformtype_id => Cvterm */
$handler->display->display_options['relationships']['platformtype_id_to_cvterm']['id'] = 'platformtype_id_to_cvterm';
$handler->display->display_options['relationships']['platformtype_id_to_cvterm']['table'] = 'arraydesign';
$handler->display->display_options['relationships']['platformtype_id_to_cvterm']['field'] = 'platformtype_id_to_cvterm';
/* Relationship: Chado Arraydesign: substratetype_id => Cvterm */
$handler->display->display_options['relationships']['substratetype_id_to_cvterm']['id'] = 'substratetype_id_to_cvterm';
$handler->display->display_options['relationships']['substratetype_id_to_cvterm']['table'] = 'arraydesign';
$handler->display->display_options['relationships']['substratetype_id_to_cvterm']['field'] = 'substratetype_id_to_cvterm';
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Chado Arraydesign: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'arraydesign';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['label'] = 'Array Design';
$handler->display->display_options['fields']['name_1']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name_1']['alter']['path'] = '[path]';
/* Field: Chado Contact: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'contact';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['label'] = 'Manufacturer';
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name_2']['id'] = 'name_2';
$handler->display->display_options['fields']['name_2']['table'] = 'cvterm';
$handler->display->display_options['fields']['name_2']['field'] = 'name';
$handler->display->display_options['fields']['name_2']['relationship'] = 'platformtype_id_to_cvterm';
$handler->display->display_options['fields']['name_2']['label'] = 'Platform';
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name_3']['id'] = 'name_3';
$handler->display->display_options['fields']['name_3']['table'] = 'cvterm';
$handler->display->display_options['fields']['name_3']['field'] = 'name';
$handler->display->display_options['fields']['name_3']['relationship'] = 'substratetype_id_to_cvterm';
$handler->display->display_options['fields']['name_3']['label'] = 'Substrate';
/* Field: Content: Delete link */
$handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['relationship'] = 'arraydesign_id';
$handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
/* Field: Content: Edit link */
$handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['relationship'] = 'arraydesign_id';
$handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = '';
$handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node] [delete_node]';
$handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
/* Sort criterion: Chado Arraydesign: Name */
$handler->display->display_options['sorts']['name']['id'] = 'name';
$handler->display->display_options['sorts']['name']['table'] = 'arraydesign';
$handler->display->display_options['sorts']['name']['field'] = 'name';
/* Filter criterion: Chado Arraydesign: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'arraydesign';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['operator'] = 'contains';
$handler->display->display_options['filters']['name']['group'] = 1;
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Array Design';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Chado Contact: Name */
$handler->display->display_options['filters']['name_2']['id'] = 'name_2';
$handler->display->display_options['filters']['name_2']['table'] = 'contact';
$handler->display->display_options['filters']['name_2']['field'] = 'name';
$handler->display->display_options['filters']['name_2']['relationship'] = 'manufacturer_id_to_contact';
$handler->display->display_options['filters']['name_2']['operator'] = 'contains';
$handler->display->display_options['filters']['name_2']['group'] = 1;
$handler->display->display_options['filters']['name_2']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_2']['expose']['operator_id'] = 'name_2_op';
$handler->display->display_options['filters']['name_2']['expose']['label'] = 'Manufacturer';
$handler->display->display_options['filters']['name_2']['expose']['operator'] = 'name_2_op';
$handler->display->display_options['filters']['name_2']['expose']['identifier'] = 'name_2';
$handler->display->display_options['filters']['name_2']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_2']['max_length'] = '40';
/* Filter criterion: Chado Cv: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'cv';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['relationship'] = 'platformtype_id_to_cvterm';
$handler->display->display_options['filters']['name_1']['operator'] = 'contains';
$handler->display->display_options['filters']['name_1']['group'] = 1;
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Platform';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';
/* Filter criterion: Chado Cvterm: Name */
$handler->display->display_options['filters']['name_3']['id'] = 'name_3';
$handler->display->display_options['filters']['name_3']['table'] = 'cvterm';
$handler->display->display_options['filters']['name_3']['field'] = 'name';
$handler->display->display_options['filters']['name_3']['relationship'] = 'substratetype_id_to_cvterm';
$handler->display->display_options['filters']['name_3']['operator'] = 'contains';
$handler->display->display_options['filters']['name_3']['group'] = 1;
$handler->display->display_options['filters']['name_3']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_3']['expose']['operator_id'] = 'name_3_op';
$handler->display->display_options['filters']['name_3']['expose']['label'] = 'Substrate';
$handler->display->display_options['filters']['name_3']['expose']['operator'] = 'name_3_op';
$handler->display->display_options['filters']['name_3']['expose']['identifier'] = 'name_3';
$handler->display->display_options['filters']['name_3']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_3']['group_info']['label'] = 'Name';
$handler->display->display_options['filters']['name_3']['group_info']['identifier'] = 'name_3';
$handler->display->display_options['filters']['name_3']['group_info']['remember'] = FALSE;
$handler->display->display_options['filters']['name_3']['group_info']['group_items'] = array(
  1 => array(),
  2 => array(),
  3 => array(),
);
$handler->display->display_options['filters']['name_3']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/tripal/extension/tripal_analysis_expression/content_types/arraydesign/arraydesigns';
$handler->display->display_options['menu']['type'] = 'default tab';
$handler->display->display_options['menu']['title'] = 'Array Design';
$handler->display->display_options['menu']['description'] = 'An array design.';
$handler->display->display_options['menu']['weight'] = '-10';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;  
}

/**
 * This function contains the default view for the protocol
 * content type for admin users. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_protocol_admin_defaultview_protocols() {

$view = new view();
$view->name = 'tripal_protocol_admin_protocols';
$view->description = 'DO NOT DISABLE';
$view->tag = 'tripal admin';
$view->base_table = 'protocol';
$view->human_name = 'Protocol Admin';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Protocol';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer tripal analysis expression';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['style_plugin'] = 'table';
/* Header: Global: Action Links */
$handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
$handler->display->display_options['header']['action_links_area']['table'] = 'views';
$handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
$handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
$handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
$handler->display->display_options['header']['action_links_area']['link-1'] = array(
  'label-1' => 'Add Protocol',
  'path-1' => '/node/add/chado-protocol',
);
$handler->display->display_options['header']['action_links_area']['link-2'] = array(
  'label-2' => '',
  'path-2' => '',
);
$handler->display->display_options['header']['action_links_area']['link-3'] = array(
  'label-3' => '',
  'path-3' => '',
);
$handler->display->display_options['header']['action_links_area']['link-4'] = array(
  'label-4' => '',
  'path-4' => '',
);
/* Relationship: Chado Protocol: pub_id => Pub (pub_id) */
$handler->display->display_options['relationships']['pub_id_to_pub']['id'] = 'pub_id_to_pub';
$handler->display->display_options['relationships']['pub_id_to_pub']['table'] = 'protocol';
$handler->display->display_options['relationships']['pub_id_to_pub']['field'] = 'pub_id_to_pub';
/* Relationship: Chado Protocol: type_id => Cvterm (cvterm_id) */
$handler->display->display_options['relationships']['type_id_to_cvterm']['id'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['table'] = 'protocol';
$handler->display->display_options['relationships']['type_id_to_cvterm']['field'] = 'type_id_to_cvterm';
/* Relationship: Protocol: Protocol => Node */
$handler->display->display_options['relationships']['protocol_id']['id'] = 'protocol_id';
$handler->display->display_options['relationships']['protocol_id']['table'] = 'chado_protocol';
$handler->display->display_options['relationships']['protocol_id']['field'] = 'protocol_id';
$handler->display->display_options['relationships']['protocol_id']['required'] = TRUE;
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Chado Protocol: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'protocol';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['path'] = '[path]';
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'cvterm';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['relationship'] = 'type_id_to_cvterm';
$handler->display->display_options['fields']['name_1']['label'] = 'Protocol Type';
/* Field: Chado Protocol: Uri */
$handler->display->display_options['fields']['uri']['id'] = 'uri';
$handler->display->display_options['fields']['uri']['table'] = 'protocol';
$handler->display->display_options['fields']['uri']['field'] = 'uri';
$handler->display->display_options['fields']['uri']['label'] = 'Origin of Protocol';
$handler->display->display_options['fields']['uri']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['uri']['alter']['text'] = 'link';
$handler->display->display_options['fields']['uri']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['uri']['alter']['path'] = '[uri]';
/* Field: Content: Delete link */
$handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['relationship'] = 'protocol_id';
$handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
/* Field: Content: Edit link */
$handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['relationship'] = 'protocol_id';
$handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['alter']['text'] = '[edit_node] [delete_node]';
/* Filter criterion: Chado Protocol: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'protocol';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['operator'] = 'word';
$handler->display->display_options['filters']['name']['group'] = 1;
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Protocol';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Chado Cvterm: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'cvterm';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['relationship'] = 'type_id_to_cvterm';
$handler->display->display_options['filters']['name_1']['operator'] = 'word';
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Protocol Type';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/tripal/extension/tripal_analysis_expression/content_types/protocol/protocols';
$handler->display->display_options['menu']['type'] = 'default tab';
$handler->display->display_options['menu']['title'] = 'Protocols';
$handler->display->display_options['menu']['description'] = 'A list of protocols. ';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;
}
 
/**
 * This function contains the default view for the protocol
 * content type for anonymous users. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_protocol_defaultvalue_user_protocols() {

$view = new view();
$view->name = 'tripal_protocol_user_protocols';
$view->description = 'Allows users to search protocol data';
$view->tag = 'tripal search';
$view->base_table = 'protocol';
$view->human_name = 'Protocol User Search';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Protocol';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['style_plugin'] = 'table';
/* Relationship: Chado Protocol: pub_id => Pub */
$handler->display->display_options['relationships']['pub_id_to_pub']['id'] = 'pub_id_to_pub';
$handler->display->display_options['relationships']['pub_id_to_pub']['table'] = 'protocol';
$handler->display->display_options['relationships']['pub_id_to_pub']['field'] = 'pub_id_to_pub';
/* Relationship: Chado Protocol: type_id => Cvterm */
$handler->display->display_options['relationships']['type_id_to_cvterm']['id'] = 'type_id_to_cvterm';
$handler->display->display_options['relationships']['type_id_to_cvterm']['table'] = 'protocol';
$handler->display->display_options['relationships']['type_id_to_cvterm']['field'] = 'type_id_to_cvterm';
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Chado Protocol: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'protocol';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['path'] = '[path]';
/* Field: Chado Cvterm: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'cvterm';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['relationship'] = 'type_id_to_cvterm';
$handler->display->display_options['fields']['name_1']['label'] = 'Protocol Type';
/* Field: Chado Protocol: Uri */
$handler->display->display_options['fields']['uri']['id'] = 'uri';
$handler->display->display_options['fields']['uri']['table'] = 'protocol';
$handler->display->display_options['fields']['uri']['field'] = 'uri';
$handler->display->display_options['fields']['uri']['label'] = 'Origin of Protocol';
$handler->display->display_options['fields']['uri']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['uri']['alter']['text'] = 'link';
$handler->display->display_options['fields']['uri']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['uri']['alter']['path'] = '[uri]';
/* Filter criterion: Chado Protocol: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'protocol';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['operator'] = 'word';
$handler->display->display_options['filters']['name']['group'] = 1;
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Protocol';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Chado Cvterm: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'cvterm';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['relationship'] = 'type_id_to_cvterm';
$handler->display->display_options['filters']['name_1']['operator'] = 'word';
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Protocol Type';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'chado/protocol';
$handler->display->display_options['menu']['title'] = 'Protocols';
$handler->display->display_options['menu']['description'] = 'A list of protocols. ';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;
}

/**
 * This function contains the default view for the biomaterial
 * content type for admin users. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_biomaterial_admin_defaultview_biomaterials() {

$view = new view();
$view->name = 'tripal_biomaterial_admin_biomaterials';
$view->description = 'DO NOT DISABLE';
$view->tag = 'tripal admin';
$view->base_table = 'biomaterial';
$view->human_name = 'Biomaterial Admin';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Biomaterials';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'administer tripal analysis expression';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'path' => 'path',
  'path_1' => 'path_1',
  'path_2' => 'path_2',
  'name' => 'name',
  'common_name' => 'common_name',
  'genus' => 'genus',
  'species' => 'species',
  'nothing' => 'nothing',
  'name_1' => 'name_1',
  'delete_node' => 'delete_node',
  'edit_node' => 'edit_node',
  'nothing_1' => 'nothing_1',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'path' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'path_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'path_2' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'common_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'genus' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'species' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nothing' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name_1' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'delete_node' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'edit_node' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nothing_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Action Links */
$handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
$handler->display->display_options['header']['action_links_area']['table'] = 'views';
$handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
$handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
$handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
$handler->display->display_options['header']['action_links_area']['link-1'] = array(
  'label-1' => 'Add Biomaterial',
  'path-1' => '/node/add/chado-biomaterial',
);
$handler->display->display_options['header']['action_links_area']['link-2'] = array(
  'label-2' => '',
  'path-2' => '',
);
$handler->display->display_options['header']['action_links_area']['link-3'] = array(
  'label-3' => '',
  'path-3' => '',
);
$handler->display->display_options['header']['action_links_area']['link-4'] = array(
  'label-4' => '',
  'path-4' => '',
);
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'No biomaterials matched the supplied criteria.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Relationship: Biomaterial: Biomaterial => Node */
$handler->display->display_options['relationships']['biomaterial_id']['id'] = 'biomaterial_id';
$handler->display->display_options['relationships']['biomaterial_id']['table'] = 'chado_biomaterial';
$handler->display->display_options['relationships']['biomaterial_id']['field'] = 'biomaterial_id';
$handler->display->display_options['relationships']['biomaterial_id']['required'] = TRUE;
/* Relationship: Chado Biomaterial: taxon_id => Organism (organism_id) */
$handler->display->display_options['relationships']['taxon_id_to_organism']['id'] = 'taxon_id_to_organism';
$handler->display->display_options['relationships']['taxon_id_to_organism']['table'] = 'biomaterial';
$handler->display->display_options['relationships']['taxon_id_to_organism']['field'] = 'taxon_id_to_organism';
/* Relationship: Chado Biomaterial: biosourceprovider_id => Contact (contact_id) */
$handler->display->display_options['relationships']['biosourceprovider_id_to_contact']['id'] = 'biosourceprovider_id_to_contact';
$handler->display->display_options['relationships']['biosourceprovider_id_to_contact']['table'] = 'biomaterial';
$handler->display->display_options['relationships']['biosourceprovider_id_to_contact']['field'] = 'biosourceprovider_id_to_contact';
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['relationship'] = 'biomaterial_id';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Content: Path */
$handler->display->display_options['fields']['path_1']['id'] = 'path_1';
$handler->display->display_options['fields']['path_1']['table'] = 'node';
$handler->display->display_options['fields']['path_1']['field'] = 'path';
$handler->display->display_options['fields']['path_1']['relationship'] = 'taxon_id_to_organism';
$handler->display->display_options['fields']['path_1']['exclude'] = TRUE;
/* Field: Chado Biomaterial: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'biomaterial';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['label'] = 'Biomaterial';
$handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['path'] = '[path]';
/* Field: Chado Organism: Common Name */
$handler->display->display_options['fields']['common_name']['id'] = 'common_name';
$handler->display->display_options['fields']['common_name']['table'] = 'organism';
$handler->display->display_options['fields']['common_name']['field'] = 'common_name';
$handler->display->display_options['fields']['common_name']['exclude'] = TRUE;
/* Field: Chado Organism: Genus */
$handler->display->display_options['fields']['genus']['id'] = 'genus';
$handler->display->display_options['fields']['genus']['table'] = 'organism';
$handler->display->display_options['fields']['genus']['field'] = 'genus';
$handler->display->display_options['fields']['genus']['exclude'] = TRUE;
/* Field: Chado Organism: Species */
$handler->display->display_options['fields']['species']['id'] = 'species';
$handler->display->display_options['fields']['species']['table'] = 'organism';
$handler->display->display_options['fields']['species']['field'] = 'species';
$handler->display->display_options['fields']['species']['exclude'] = TRUE;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'Organism';
$handler->display->display_options['fields']['nothing']['alter']['text'] = '[genus] [species] ([common_name])';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = '[path_1]';
/* Field: Chado Contact: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'contact';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['label'] = 'Biomaterial Provider';
/* Field: Content: Delete link */
$handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['relationship'] = 'biomaterial_id';
$handler->display->display_options['fields']['delete_node']['exclude'] = TRUE;
/* Field: Content: Edit link */
$handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['relationship'] = 'biomaterial_id';
$handler->display->display_options['fields']['edit_node']['exclude'] = TRUE;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
$handler->display->display_options['fields']['nothing_1']['table'] = 'views';
$handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing_1']['label'] = '';
$handler->display->display_options['fields']['nothing_1']['alter']['text'] = '[edit_node] [delete_node]';
/* Sort criterion: Chado Biomaterial: Name */
$handler->display->display_options['sorts']['name']['id'] = 'name';
$handler->display->display_options['sorts']['name']['table'] = 'biomaterial';
$handler->display->display_options['sorts']['name']['field'] = 'name';
$handler->display->display_options['sorts']['name']['expose']['label'] = 'Name';
/* Sort criterion: Chado Organism: Genus */
$handler->display->display_options['sorts']['genus']['id'] = 'genus';
$handler->display->display_options['sorts']['genus']['table'] = 'organism';
$handler->display->display_options['sorts']['genus']['field'] = 'genus';
$handler->display->display_options['sorts']['genus']['relationship'] = 'taxon_id_to_organism';
/* Sort criterion: Chado Organism: Species */
$handler->display->display_options['sorts']['species']['id'] = 'species';
$handler->display->display_options['sorts']['species']['table'] = 'organism';
$handler->display->display_options['sorts']['species']['field'] = 'species';
$handler->display->display_options['sorts']['species']['relationship'] = 'taxon_id_to_organism';
/* Sort criterion: Chado Organism: Common Name */
$handler->display->display_options['sorts']['common_name']['id'] = 'common_name';
$handler->display->display_options['sorts']['common_name']['table'] = 'organism';
$handler->display->display_options['sorts']['common_name']['field'] = 'common_name';
$handler->display->display_options['sorts']['common_name']['relationship'] = 'taxon_id_to_organism';
/* Sort criterion: Chado Contact: Name */
$handler->display->display_options['sorts']['name_1']['id'] = 'name_1';
$handler->display->display_options['sorts']['name_1']['table'] = 'contact';
$handler->display->display_options['sorts']['name_1']['field'] = 'name';
$handler->display->display_options['sorts']['name_1']['relationship'] = 'biosourceprovider_id_to_contact';
/* Filter criterion: Chado Biomaterial: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'biomaterial';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Biomaterial';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Global: Combine fields filter */
$handler->display->display_options['filters']['combine']['id'] = 'combine';
$handler->display->display_options['filters']['combine']['table'] = 'views';
$handler->display->display_options['filters']['combine']['field'] = 'combine';
$handler->display->display_options['filters']['combine']['operator'] = 'word';
$handler->display->display_options['filters']['combine']['exposed'] = TRUE;
$handler->display->display_options['filters']['combine']['expose']['operator_id'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['label'] = 'Organism';
$handler->display->display_options['filters']['combine']['expose']['operator'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['identifier'] = 'combine';
$handler->display->display_options['filters']['combine']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['combine']['fields'] = array(
  'common_name' => 'common_name',
  'genus' => 'genus',
  'species' => 'species',
  'nothing' => 'nothing',
);
/* Filter criterion: Chado Contact: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'contact';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['relationship'] = 'biosourceprovider_id_to_contact';
$handler->display->display_options['filters']['name_1']['operator'] = 'word';
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Biomaterial Provider';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/tripal/extension/tripal_analysis_expression/content_types/biomaterial/biomaterials';
$handler->display->display_options['menu']['type'] = 'default tab';
$handler->display->display_options['menu']['title'] = 'Biomaterials';
$handler->display->display_options['menu']['description'] = 'A list of biomaterials.';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;
}

/**
 * This function contains the default view for the biomaterial
 * content type for anonymous users. 
 * 
 * @ingroup tripal_analysis_expression  
 */ 
function tripal_biomaterial_defaultvalue_user_biomaterials() {

$view = new view();
$view->name = 'tripal_biomaterial_user_biomaterials';
$view->description = 'Allows users to search biomaterials.';
$view->tag = 'tripal search';
$view->base_table = 'biomaterial';
$view->human_name = 'Biomaterial User Search';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Biomaterials';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '25';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'path' => 'path',
  'path_1' => 'path_1',
  'path_2' => 'path_2',
  'name' => 'name',
  'common_name' => 'common_name',
  'genus' => 'genus',
  'species' => 'species',
  'nothing' => 'nothing',
  'name_1' => 'name_1',
  'delete_node' => 'delete_node',
  'edit_node' => 'edit_node',
  'nothing_1' => 'nothing_1',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'path' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'path_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'path_2' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'common_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'genus' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'species' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nothing' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name_1' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'delete_node' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'edit_node' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nothing_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'No biomaterials matched the supplied criteria.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Relationship: Biomaterial: Biomaterial => Node */
$handler->display->display_options['relationships']['biomaterial_id']['id'] = 'biomaterial_id';
$handler->display->display_options['relationships']['biomaterial_id']['table'] = 'chado_biomaterial';
$handler->display->display_options['relationships']['biomaterial_id']['field'] = 'biomaterial_id';
$handler->display->display_options['relationships']['biomaterial_id']['required'] = TRUE;
/* Relationship: Chado Biomaterial: taxon_id => Organism (organism_id) */
$handler->display->display_options['relationships']['taxon_id_to_organism']['id'] = 'taxon_id_to_organism';
$handler->display->display_options['relationships']['taxon_id_to_organism']['table'] = 'biomaterial';
$handler->display->display_options['relationships']['taxon_id_to_organism']['field'] = 'taxon_id_to_organism';
/* Relationship: Chado Biomaterial: biosourceprovider_id => Contact (contact_id) */
$handler->display->display_options['relationships']['biosourceprovider_id_to_contact']['id'] = 'biosourceprovider_id_to_contact';
$handler->display->display_options['relationships']['biosourceprovider_id_to_contact']['table'] = 'biomaterial';
$handler->display->display_options['relationships']['biosourceprovider_id_to_contact']['field'] = 'biosourceprovider_id_to_contact';
/* Field: Content: Path */
$handler->display->display_options['fields']['path']['id'] = 'path';
$handler->display->display_options['fields']['path']['table'] = 'node';
$handler->display->display_options['fields']['path']['field'] = 'path';
$handler->display->display_options['fields']['path']['relationship'] = 'biomaterial_id';
$handler->display->display_options['fields']['path']['exclude'] = TRUE;
/* Field: Content: Path */
$handler->display->display_options['fields']['path_1']['id'] = 'path_1';
$handler->display->display_options['fields']['path_1']['table'] = 'node';
$handler->display->display_options['fields']['path_1']['field'] = 'path';
$handler->display->display_options['fields']['path_1']['relationship'] = 'taxon_id_to_organism';
$handler->display->display_options['fields']['path_1']['exclude'] = TRUE;
/* Field: Chado Biomaterial: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'biomaterial';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['label'] = 'Biomaterial';
$handler->display->display_options['fields']['name']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['path'] = '[path]';
/* Field: Chado Organism: Common Name */
$handler->display->display_options['fields']['common_name']['id'] = 'common_name';
$handler->display->display_options['fields']['common_name']['table'] = 'organism';
$handler->display->display_options['fields']['common_name']['field'] = 'common_name';
$handler->display->display_options['fields']['common_name']['exclude'] = TRUE;
/* Field: Chado Organism: Genus */
$handler->display->display_options['fields']['genus']['id'] = 'genus';
$handler->display->display_options['fields']['genus']['table'] = 'organism';
$handler->display->display_options['fields']['genus']['field'] = 'genus';
$handler->display->display_options['fields']['genus']['exclude'] = TRUE;
/* Field: Chado Organism: Species */
$handler->display->display_options['fields']['species']['id'] = 'species';
$handler->display->display_options['fields']['species']['table'] = 'organism';
$handler->display->display_options['fields']['species']['field'] = 'species';
$handler->display->display_options['fields']['species']['exclude'] = TRUE;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'Organism';
$handler->display->display_options['fields']['nothing']['alter']['text'] = '[genus] [species] ([common_name])';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = '[path_1]';
/* Field: Chado Contact: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'contact';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['label'] = 'Biomaterial Provider';
$handler->display->display_options['fields']['name_1']['alter']['path'] = '[path_2]';
/* Sort criterion: Chado Biomaterial: Name */
$handler->display->display_options['sorts']['name']['id'] = 'name';
$handler->display->display_options['sorts']['name']['table'] = 'biomaterial';
$handler->display->display_options['sorts']['name']['field'] = 'name';
$handler->display->display_options['sorts']['name']['expose']['label'] = 'Name';
/* Sort criterion: Chado Organism: Genus */
$handler->display->display_options['sorts']['genus']['id'] = 'genus';
$handler->display->display_options['sorts']['genus']['table'] = 'organism';
$handler->display->display_options['sorts']['genus']['field'] = 'genus';
$handler->display->display_options['sorts']['genus']['relationship'] = 'taxon_id_to_organism';
/* Sort criterion: Chado Organism: Species */
$handler->display->display_options['sorts']['species']['id'] = 'species';
$handler->display->display_options['sorts']['species']['table'] = 'organism';
$handler->display->display_options['sorts']['species']['field'] = 'species';
$handler->display->display_options['sorts']['species']['relationship'] = 'taxon_id_to_organism';
/* Sort criterion: Chado Organism: Common Name */
$handler->display->display_options['sorts']['common_name']['id'] = 'common_name';
$handler->display->display_options['sorts']['common_name']['table'] = 'organism';
$handler->display->display_options['sorts']['common_name']['field'] = 'common_name';
$handler->display->display_options['sorts']['common_name']['relationship'] = 'taxon_id_to_organism';
/* Sort criterion: Chado Contact: Name */
$handler->display->display_options['sorts']['name_1']['id'] = 'name_1';
$handler->display->display_options['sorts']['name_1']['table'] = 'contact';
$handler->display->display_options['sorts']['name_1']['field'] = 'name';
$handler->display->display_options['sorts']['name_1']['relationship'] = 'biosourceprovider_id_to_contact';
/* Filter criterion: Chado Biomaterial: Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'biomaterial';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Biomaterial';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name']['max_length'] = '40';
/* Filter criterion: Global: Combine fields filter */
$handler->display->display_options['filters']['combine']['id'] = 'combine';
$handler->display->display_options['filters']['combine']['table'] = 'views';
$handler->display->display_options['filters']['combine']['field'] = 'combine';
$handler->display->display_options['filters']['combine']['operator'] = 'word';
$handler->display->display_options['filters']['combine']['exposed'] = TRUE;
$handler->display->display_options['filters']['combine']['expose']['operator_id'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['label'] = 'Organism';
$handler->display->display_options['filters']['combine']['expose']['operator'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['identifier'] = 'combine';
$handler->display->display_options['filters']['combine']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['combine']['fields'] = array(
  'common_name' => 'common_name',
  'genus' => 'genus',
  'species' => 'species',
  'nothing' => 'nothing',
);
/* Filter criterion: Chado Contact: Name */
$handler->display->display_options['filters']['name_1']['id'] = 'name_1';
$handler->display->display_options['filters']['name_1']['table'] = 'contact';
$handler->display->display_options['filters']['name_1']['field'] = 'name';
$handler->display->display_options['filters']['name_1']['relationship'] = 'biosourceprovider_id_to_contact';
$handler->display->display_options['filters']['name_1']['operator'] = 'word';
$handler->display->display_options['filters']['name_1']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_1']['expose']['operator_id'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['label'] = 'Biomaterial Provider';
$handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
$handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
$handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
$handler->display->display_options['filters']['name_1']['max_length'] = '40';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'chado/biomaterial';
$handler->display->display_options['menu']['title'] = 'Biomaterials';
$handler->display->display_options['menu']['description'] = 'A list of biomaterials.';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$handler->display->display_options['tab_options']['weight'] = '0';

return $view;
}

