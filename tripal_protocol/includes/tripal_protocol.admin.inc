<?php
/**
 * @file
 *
 * This file contains teh functions used for the administration
 * of the protocol content type.
 *
 * @ingroup tripal_protocol
 */

/**
 * Administration page callbacks for the protocol content type.
 *
 * @return
 *   A form API array describing an adminsitrative form.
 *
 * @ingroup tripal_protocol
 */
function tripal_protocol_admin() {

  $form = [];

  $details = [
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_protocol',
    'options' => [
      '[protocol.name]' => 'Protocol Only',
    ],
    'unique_option' => '[protocol.name]' //*** fix  me
  ];
  chado_add_admin_form_set_title($form, $form_state, $details);

  $details = [
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_protocol',
    'options' => [ //*** fix these
      '/protocol/[protocol.protocol_id]' => 'Protocol ID',
      '/protocol/[protocol.name]' => 'Protocol Name',
    ],
  ];
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}

/**
 * Landing page for administration. Ensures Views are enabled and if not,
 * provides links to do so.
 *
 * @return
 *   An HTML string that provides a link to enable the view.
 *
 * @ingroup tripal_protocol
 */
function tripal_protocol_admin_protocol_view() {
  $output = '';

  // Set the breadcrumbs. 
  $breadcrumb = []; //*** update this for tripal
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
  $breadcrumb[] = l('Tripal Expression', 'admin/tripal/extension/tripal_analysis_expression');
  $breadcrumb[] = l('Analysis: Expression Content Types', 'admin/tripal/extension/tripal_analysis_expression/content_types');
  $breadcrumb[] = l('Protocols', 'admin/tripal/extension/tripal_analysis_expression/content_types/protocol');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view. 
  $view = views_embed_view('tripal_protocol_admin_protocols', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Expression module uses primarily views to provide an ' . 'administrative interface. Currently one or more views needed for this this ' . 'administrative interface are disabled. <br><strong>Click each of the following links to ' . 'enable the pertinent views</strong>:</b>'; //*** fix this wording
    $output .= '<ul>';
    $output .= '<li>' . l('Protocol View', 'admin/tripal/extension/tripal_analysis_expression/content_types/protocol/views/protocols/enable') . '</li>';
    $output .= '</ul>';
  }
  return $output;
}




