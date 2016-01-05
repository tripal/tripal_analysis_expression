<?php
/**
 * @file
 * 
 * This file contains the functions used for the administration 
 * of the arraydesign content type.
 * 
 * @ingroup tripal_arraydesign
 */

/**
 * Administration page callbacks for the arraydesign content type.
 *
 * @return
 *   A form API array describing an administrative form.
 *
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_admin() {

  $form = array();
  
  $details = array(
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_arraydesign',
    'options' => array(
      '[arraydesign.name]' => 'Array Design Only'
    ),
    'unique_option' => '[arraydesign.name]'
  );
  chado_add_admin_form_set_title($form, $form_state, $details);

  $details = array(
    'module' => 'tripal_analysis_expression',
    'content_type' => 'chado_arraydesign',
    'options' => array( //*** fix these
      '/arraydesign/[arraydesign.arraydesign_id]' => 'Array Design ID',
      '/arraydesign/[arraydesign.name]' => 'Array Design Name'
    ),
  );
  chado_add_admin_form_set_url($form, $form_state, $details);

 return system_settings_form($form);
}

/**
 * Landing page for administration. Ensures Views are enabled and if not, provides links to do so. 
 *
 * @return
 *   An HTML string that provides a link to enable the view.
 *  
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_admin_arraydesign_view() {

  $output = '';
  
  // Set the breadcrumbs. 
  $breadcrumb = array(); //*** update this for tripal
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
  $breadcrumb[] = l('Tripal Expression', 'admin/tripal/extension/tripal_analysis_expression');
  $breadcrumb[] = l('Analysis: Expression Content Types', 'admin/tripal/extension/tripal_analysis_expression/content_types');
  $breadcrumb[] = l('Array Design', 'admin/tripal/extension/tripal_analysis_expression/content_types/arraydesign');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view. 
  $view = views_embed_view('tripal_arraydesign_admin_arraydesigns', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else { 
    $output .= '<p>The Tripal Expression module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this this ' 
      . 'administrative interface are disabled. <br><strong>Click each of the following links to ' 
      . 'enable the pertinent views</strong>:</b>'; //*** fix this wording
    $output .= '<ul>';
    $output .= '<li>' . l('Arraydesign View', 'admin/tripal/extension/tripal_analysis_expression/content_type/arraydesign/views/arraydesigns/enable') . '</li>';
    $output .= '</ul>';
  }
  return $output;
}

