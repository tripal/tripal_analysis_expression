<?php
/**
 * @file
 *
 * This file contains the functions used for the administration
 * of the biomaterial content type.
 *
 * @ingroup tripal_biomaterial
 */

/**
 * Administrative page callbacks for the biomaterial content type.
 *
 * @return
 *   A form API array describing an administrative form.
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_admin() {

  $form = [];

  $details = [
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_biomaterial',
    'options' => [
      '[biomaterial.name]' => 'Biomaterial Only',
    ],
    'unique_option' => '[biomaterial.name]' //*** fix  me
  ];
  chado_add_admin_form_set_title($form, $form_state, $details);

  $details = [
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_biomaterial',
    'options' => [ //*** fix these
      '/biomaterial/[biomaterial.biomaterial_id]' => 'Biomaterial ID',
      '/biomaterial/[biomaterial.name]' => 'Biomaterial Name',
    ],
  ];
  chado_add_admin_form_set_url($form, $form_state, $details);

  $form['ontologies'] = [
    '#title' => t('Biomaterial Ontology Terms'),
    '#type' => 'fieldset',
    '#description' => t('Configuration settings for ontology term display.'),
    '#collapsible' => TRUE,
  ];

  $form['ontologies']['chado_biomaterial_ontologies_to_display'] = [
    '#title' => t('Ontologies to Display'),
    '#description' => t('List the sequence ontologies for which cvterms should be displayed (if present)') . t(' on the biomaterial page. Ontologies should be separated by a space or newline.'),
    '#type' => 'textarea',
    '#default_value' => variable_get('chado_biomaterial_ontologies_to_display'),
  ];

  return system_settings_form($form);
}

/**
 * Landing page for administration. Ensures Views are enabled and if not,
 * provides links to do so.
 *
 * @return
 *   An HTML string that provides a link to enable the view.
 *
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_admin_biomaterial_view() {
  $output = '';

  // Set the breadcrumbs. 
  $breadcrumb = []; //*** update this for tripal
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
  $breadcrumb[] = l('Tripal Expression', 'admin/tripal/extension/tripal_analysis_expression');
  $breadcrumb[] = l('Analysis: Expression Content Types', 'admin/tripal/extension/tripal_analysis_expression/content_types');
  $breadcrumb[] = l('Biomaterials', 'admin/tripal/extension/tripal_analysis_expression/content_types/biomaterial');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view. 
  $view = views_embed_view('tripal_biomaterial_admin_biomaterials', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Expression module uses primarily views to provide an ' . 'administrative interface. Currently one or more views needed for this this ' . 'administrative interface are disabled. <br><strong>Click each of the following links to ' . 'enable the pertinent views</strong>:</b>'; //*** fix this wording
    $output .= '<ul>';
    $output .= '<li>' . l('Biomaterial View', 'admin/tripal/extension/tripal_analysis_expression/content_types/biomaterial/views/biomaterials/enable') . '</li>';
    $output .= '</ul>';
  }
  return $output;
}




