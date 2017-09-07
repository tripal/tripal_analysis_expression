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
  $views = [];


  $view = tripal_analysis_expression_defaultvalue_user_analysis_expressions();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_analysis_expression_admin_defaultvalue_analysis_expressions();
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
  $handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name';
  $handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
  $handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
  $handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_1']['max_length'] = '40';
  /* Filter criterion: Chado Analysis: Program */
  $handler->display->display_options['filters']['program']['id'] = 'program';
  $handler->display->display_options['filters']['program']['table'] = 'analysis';
  $handler->display->display_options['filters']['program']['field'] = 'program';
  $handler->display->display_options['filters']['program']['operator'] = 'word';
  $handler->display->display_options['filters']['program']['exposed'] = TRUE;
  $handler->display->display_options['filters']['program']['expose']['operator_id'] = 'program_op';
  $handler->display->display_options['filters']['program']['expose']['label'] = 'Program';
  $handler->display->display_options['filters']['program']['expose']['operator'] = 'program_op';
  $handler->display->display_options['filters']['program']['expose']['identifier'] = 'program';
  $handler->display->display_options['filters']['program']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['program']['group_info']['label'] = 'Program';
  $handler->display->display_options['filters']['program']['group_info']['identifier'] = 'program';
  $handler->display->display_options['filters']['program']['group_info']['remember'] = FALSE;
  $handler->display->display_options['filters']['program']['group_info']['group_items'] = [
    1 => [],
    2 => [],
    3 => [],
  ];
  /* Filter criterion: Chado Analysis: Sourcename */
  $handler->display->display_options['filters']['sourcename']['id'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['table'] = 'analysis';
  $handler->display->display_options['filters']['sourcename']['field'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['operator'] = 'word';
  $handler->display->display_options['filters']['sourcename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['sourcename']['expose']['operator_id'] = 'sourcename_op';
  $handler->display->display_options['filters']['sourcename']['expose']['label'] = 'Source';
  $handler->display->display_options['filters']['sourcename']['expose']['operator'] = 'sourcename_op';
  $handler->display->display_options['filters']['sourcename']['expose']['identifier'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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
  $handler->display->display_options['header']['action_links_area']['link-1'] = [
    'label-1' => 'Add Analysis: Expression',
    'path-1' => '/node/add/chado-analysis-expression',
  ];
  $handler->display->display_options['header']['action_links_area']['link-2'] = [
    'label-2' => '',
    'path-2' => '',
  ];
  $handler->display->display_options['header']['action_links_area']['link-3'] = [
    'label-3' => '',
    'path-3' => '',
  ];
  $handler->display->display_options['header']['action_links_area']['link-4'] = [
    'label-4' => '',
    'path-4' => '',
  ];
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
  $handler->display->display_options['filters']['name_1']['expose']['label'] = 'Name';
  $handler->display->display_options['filters']['name_1']['expose']['operator'] = 'name_1_op';
  $handler->display->display_options['filters']['name_1']['expose']['identifier'] = 'name_1';
  $handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_1']['max_length'] = '40';
  /* Filter criterion: Chado Analysis: Program */
  $handler->display->display_options['filters']['program']['id'] = 'program';
  $handler->display->display_options['filters']['program']['table'] = 'analysis';
  $handler->display->display_options['filters']['program']['field'] = 'program';
  $handler->display->display_options['filters']['program']['operator'] = 'word';
  $handler->display->display_options['filters']['program']['exposed'] = TRUE;
  $handler->display->display_options['filters']['program']['expose']['operator_id'] = 'program_op';
  $handler->display->display_options['filters']['program']['expose']['label'] = 'Program';
  $handler->display->display_options['filters']['program']['expose']['operator'] = 'program_op';
  $handler->display->display_options['filters']['program']['expose']['identifier'] = 'program';
  $handler->display->display_options['filters']['program']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['program']['group_info']['label'] = 'Program';
  $handler->display->display_options['filters']['program']['group_info']['identifier'] = 'program';
  $handler->display->display_options['filters']['program']['group_info']['remember'] = FALSE;
  $handler->display->display_options['filters']['program']['group_info']['group_items'] = [
    1 => [],
    2 => [],
    3 => [],
  ];
  /* Filter criterion: Chado Analysis: Sourcename */
  $handler->display->display_options['filters']['sourcename']['id'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['table'] = 'analysis';
  $handler->display->display_options['filters']['sourcename']['field'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['operator'] = 'word';
  $handler->display->display_options['filters']['sourcename']['exposed'] = TRUE;
  $handler->display->display_options['filters']['sourcename']['expose']['operator_id'] = 'sourcename_op';
  $handler->display->display_options['filters']['sourcename']['expose']['label'] = 'Source';
  $handler->display->display_options['filters']['sourcename']['expose']['operator'] = 'sourcename_op';
  $handler->display->display_options['filters']['sourcename']['expose']['identifier'] = 'sourcename';
  $handler->display->display_options['filters']['sourcename']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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