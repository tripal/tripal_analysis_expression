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
function tripal_protocols_views_default_views() {
  $views = [];

  $view = tripal_arraydesign_defaultvalue_user_arraydesigns();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_protocol_defaultvalue_user_protocols();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;


  $view = tripal_protocols_defaultvalue_user_analysis_expressions();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_protocols_admin_defaultvalue_analysis_expressions();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_arraydesign_admin_defaultview_arraydesigns();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  $view = tripal_protocol_admin_defaultview_protocols();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;


  return $views;
}

/**
 * This function contains the default view for the arraydesign
 * content type for anonymous users.
 *
 * @ingroup tripal_protocols
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
  $handler->display->display_options['style_options']['columns'] = [
    'name_1' => 'name_1',
    'name' => 'name',
    'name_2' => 'name_2',
    'name_3' => 'name_3',
  ];
  $handler->display->display_options['style_options']['default'] = 'name_1';
  $handler->display->display_options['style_options']['info'] = [
    'name_1' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name_2' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name_3' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
  ];
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
  /* Relationship: Chado Arraydesign: manufacturer_id => Contact (contact_id) */
  $handler->display->display_options['relationships']['manufacturer_id_to_contact']['id'] = 'manufacturer_id_to_contact';
  $handler->display->display_options['relationships']['manufacturer_id_to_contact']['table'] = 'arraydesign';
  $handler->display->display_options['relationships']['manufacturer_id_to_contact']['field'] = 'manufacturer_id_to_contact';
  /* Relationship: Chado Arraydesign: platformtype_id => Cvterm (cvterm_id) */
  $handler->display->display_options['relationships']['platformtype_id_to_cvterm']['id'] = 'platformtype_id_to_cvterm';
  $handler->display->display_options['relationships']['platformtype_id_to_cvterm']['table'] = 'arraydesign';
  $handler->display->display_options['relationships']['platformtype_id_to_cvterm']['field'] = 'platformtype_id_to_cvterm';
  /* Relationship: Chado Arraydesign: substratetype_id => Cvterm (cvterm_id) */
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
  $handler->display->display_options['filters']['name']['operator'] = 'word';
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Array Design';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name']['max_length'] = '40';
  /* Filter criterion: Chado Contact: Name */
  $handler->display->display_options['filters']['name_2']['id'] = 'name_2';
  $handler->display->display_options['filters']['name_2']['table'] = 'contact';
  $handler->display->display_options['filters']['name_2']['field'] = 'name';
  $handler->display->display_options['filters']['name_2']['relationship'] = 'manufacturer_id_to_contact';
  $handler->display->display_options['filters']['name_2']['operator'] = 'word';
  $handler->display->display_options['filters']['name_2']['group'] = 1;
  $handler->display->display_options['filters']['name_2']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_2']['expose']['operator_id'] = 'name_2_op';
  $handler->display->display_options['filters']['name_2']['expose']['label'] = 'Manufacturer';
  $handler->display->display_options['filters']['name_2']['expose']['operator'] = 'name_2_op';
  $handler->display->display_options['filters']['name_2']['expose']['identifier'] = 'name_2';
  $handler->display->display_options['filters']['name_2']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_2']['max_length'] = '40';
  /* Filter criterion: Chado Cvterm: Name */
  $handler->display->display_options['filters']['name_4']['id'] = 'name_4';
  $handler->display->display_options['filters']['name_4']['table'] = 'cvterm';
  $handler->display->display_options['filters']['name_4']['field'] = 'name';
  $handler->display->display_options['filters']['name_4']['relationship'] = 'platformtype_id_to_cvterm';
  $handler->display->display_options['filters']['name_4']['operator'] = 'word';
  $handler->display->display_options['filters']['name_4']['group'] = 1;
  $handler->display->display_options['filters']['name_4']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_4']['expose']['operator_id'] = 'name_4_op';
  $handler->display->display_options['filters']['name_4']['expose']['label'] = 'Platform';
  $handler->display->display_options['filters']['name_4']['expose']['operator'] = 'name_4_op';
  $handler->display->display_options['filters']['name_4']['expose']['identifier'] = 'name_4';
  $handler->display->display_options['filters']['name_4']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_4']['max_length'] = '40';
  /* Filter criterion: Chado Cvterm: Name */
  $handler->display->display_options['filters']['name_3']['id'] = 'name_3';
  $handler->display->display_options['filters']['name_3']['table'] = 'cvterm';
  $handler->display->display_options['filters']['name_3']['field'] = 'name';
  $handler->display->display_options['filters']['name_3']['relationship'] = 'substratetype_id_to_cvterm';
  $handler->display->display_options['filters']['name_3']['operator'] = 'word';
  $handler->display->display_options['filters']['name_3']['group'] = 1;
  $handler->display->display_options['filters']['name_3']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_3']['expose']['operator_id'] = 'name_3_op';
  $handler->display->display_options['filters']['name_3']['expose']['label'] = 'Substrate';
  $handler->display->display_options['filters']['name_3']['expose']['operator'] = 'name_3_op';
  $handler->display->display_options['filters']['name_3']['expose']['identifier'] = 'name_3';
  $handler->display->display_options['filters']['name_3']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_3']['group_info']['label'] = 'Name';
  $handler->display->display_options['filters']['name_3']['group_info']['identifier'] = 'name_3';
  $handler->display->display_options['filters']['name_3']['group_info']['remember'] = FALSE;
  $handler->display->display_options['filters']['name_3']['group_info']['group_items'] = [
    1 => [],
    2 => [],
    3 => [],
  ];
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
 * @ingroup tripal_protocols
 */
function tripal_arraydesign_admin_defaultview_arraydesigns() {

  $view = new view();
  $view->name = 'tripal_arraydesign_admin_arraydesigns';
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
  $handler->display->display_options['style_options']['columns'] = [
    'name_1' => 'name_1',
    'name' => 'name',
    'name_2' => 'name_2',
    'name_3' => 'name_3',
  ];
  $handler->display->display_options['style_options']['default'] = 'name_1';
  $handler->display->display_options['style_options']['info'] = [
    'name_1' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name_2' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
    'name_3' => [
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ],
  ];
  /* Header: Global: Action Links */
  $handler->display->display_options['header']['action_links_area']['id'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['table'] = 'views';
  $handler->display->display_options['header']['action_links_area']['field'] = 'action_links_area';
  $handler->display->display_options['header']['action_links_area']['label'] = 'Action Links';
  $handler->display->display_options['header']['action_links_area']['empty'] = TRUE;
  $handler->display->display_options['header']['action_links_area']['link-1'] = [
    'label-1' => 'Add Array Design',
    'path-1' => 'node/add/chado-arraydesign',
  ];
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
  /* Relationship: Chado Arraydesign: manufacturer_id => Contact (contact_id) */
  $handler->display->display_options['relationships']['manufacturer_id_to_contact']['id'] = 'manufacturer_id_to_contact';
  $handler->display->display_options['relationships']['manufacturer_id_to_contact']['table'] = 'arraydesign';
  $handler->display->display_options['relationships']['manufacturer_id_to_contact']['field'] = 'manufacturer_id_to_contact';
  /* Relationship: Chado Arraydesign: platformtype_id => Cvterm (cvterm_id) */
  $handler->display->display_options['relationships']['platformtype_id_to_cvterm']['id'] = 'platformtype_id_to_cvterm';
  $handler->display->display_options['relationships']['platformtype_id_to_cvterm']['table'] = 'arraydesign';
  $handler->display->display_options['relationships']['platformtype_id_to_cvterm']['field'] = 'platformtype_id_to_cvterm';
  /* Relationship: Chado Arraydesign: substratetype_id => Cvterm (cvterm_id) */
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
  $handler->display->display_options['filters']['name']['operator'] = 'word';
  $handler->display->display_options['filters']['name']['group'] = 1;
  $handler->display->display_options['filters']['name']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['label'] = 'Array Design';
  $handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
  $handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name']['max_length'] = '40';
  /* Filter criterion: Chado Contact: Name */
  $handler->display->display_options['filters']['name_2']['id'] = 'name_2';
  $handler->display->display_options['filters']['name_2']['table'] = 'contact';
  $handler->display->display_options['filters']['name_2']['field'] = 'name';
  $handler->display->display_options['filters']['name_2']['relationship'] = 'manufacturer_id_to_contact';
  $handler->display->display_options['filters']['name_2']['operator'] = 'word';
  $handler->display->display_options['filters']['name_2']['group'] = 1;
  $handler->display->display_options['filters']['name_2']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_2']['expose']['operator_id'] = 'name_2_op';
  $handler->display->display_options['filters']['name_2']['expose']['label'] = 'Manufacturer';
  $handler->display->display_options['filters']['name_2']['expose']['operator'] = 'name_2_op';
  $handler->display->display_options['filters']['name_2']['expose']['identifier'] = 'name_2';
  $handler->display->display_options['filters']['name_2']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_2']['max_length'] = '40';
  /* Filter criterion: Chado Cvterm: Name */
  $handler->display->display_options['filters']['name_4']['id'] = 'name_4';
  $handler->display->display_options['filters']['name_4']['table'] = 'cvterm';
  $handler->display->display_options['filters']['name_4']['field'] = 'name';
  $handler->display->display_options['filters']['name_4']['relationship'] = 'platformtype_id_to_cvterm';
  $handler->display->display_options['filters']['name_4']['operator'] = 'word';
  $handler->display->display_options['filters']['name_4']['group'] = 1;
  $handler->display->display_options['filters']['name_4']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_4']['expose']['operator_id'] = 'name_4_op';
  $handler->display->display_options['filters']['name_4']['expose']['label'] = 'Platform';
  $handler->display->display_options['filters']['name_4']['expose']['operator'] = 'name_4_op';
  $handler->display->display_options['filters']['name_4']['expose']['identifier'] = 'name_4';
  $handler->display->display_options['filters']['name_4']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_4']['max_length'] = '40';
  /* Filter criterion: Chado Cvterm: Name */
  $handler->display->display_options['filters']['name_3']['id'] = 'name_3';
  $handler->display->display_options['filters']['name_3']['table'] = 'cvterm';
  $handler->display->display_options['filters']['name_3']['field'] = 'name';
  $handler->display->display_options['filters']['name_3']['relationship'] = 'substratetype_id_to_cvterm';
  $handler->display->display_options['filters']['name_3']['operator'] = 'word';
  $handler->display->display_options['filters']['name_3']['group'] = 1;
  $handler->display->display_options['filters']['name_3']['exposed'] = TRUE;
  $handler->display->display_options['filters']['name_3']['expose']['operator_id'] = 'name_3_op';
  $handler->display->display_options['filters']['name_3']['expose']['label'] = 'Substrate';
  $handler->display->display_options['filters']['name_3']['expose']['operator'] = 'name_3_op';
  $handler->display->display_options['filters']['name_3']['expose']['identifier'] = 'name_3';
  $handler->display->display_options['filters']['name_3']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_3']['group_info']['label'] = 'Name';
  $handler->display->display_options['filters']['name_3']['group_info']['identifier'] = 'name_3';
  $handler->display->display_options['filters']['name_3']['group_info']['remember'] = FALSE;
  $handler->display->display_options['filters']['name_3']['group_info']['group_items'] = [
    1 => [],
    2 => [],
    3 => [],
  ];
  $handler->display->display_options['filters']['name_3']['max_length'] = '40';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/extension/tripal_protocols/content_types/arraydesign/arraydesigns';
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
 * @ingroup tripal_protocols
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
  $handler->display->display_options['header']['action_links_area']['link-1'] = [
    'label-1' => 'Add Protocol',
    'path-1' => '/node/add/chado-protocol',
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
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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
  $handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
  $handler->display->display_options['filters']['name_1']['max_length'] = '40';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/tripal/extension/tripal_protocols/content_types/protocol/protocols';
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
 * @ingroup tripal_protocols
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
  /* Relationship: Chado Protocol: pub_id => Pub (pub_id) */
  $handler->display->display_options['relationships']['pub_id_to_pub']['id'] = 'pub_id_to_pub';
  $handler->display->display_options['relationships']['pub_id_to_pub']['table'] = 'protocol';
  $handler->display->display_options['relationships']['pub_id_to_pub']['field'] = 'pub_id_to_pub';
  /* Relationship: Chado Protocol: type_id => Cvterm (cvterm_id) */
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
  $handler->display->display_options['filters']['name']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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
  $handler->display->display_options['filters']['name_1']['expose']['remember_roles'] = [
    2 => '2',
    1 => 0,
    3 => 0,
  ];
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