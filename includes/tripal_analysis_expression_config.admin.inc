<?php
/**
 * @file
 * Administration of Analysis: Expression module
 */

/**
 * Implements an admin page to congigure the display of analysis: expression data.
 * 
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_config_admin() {

  // Set the breadcrumbs. 
  $breadcrumb = array(); //*** update this for tripal
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
  $breadcrumb[] = l('Tripal Expression', 'admin/tripal/extension/tripal_analysis_expression');
  $breadcrumb[] = l('Analysis: Expression Configuration', 'admin/tripal/extension/tripal_analysis_expression/manage');
  drupal_set_breadcrumb($breadcrumb);
 
  $form['feature_expression'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature Display'),
  );
  
  $form['feature_expression']['hide_expression'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide Expression Figure'),
    '#description' => t('Hide expression figure on all feature pages.'),
    '#default_value' => variable_get('chado_feature_expression_hide_expression',TRUE),
  );

  $form['feature_expression']['hide_biomaterial_labels'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide Biomaterial Labels'),
    '#description' => t('Hide biomaterials labels on expression figure.'),
    '#options' => t('Show Labels'),
    '#default_value' => variable_get('chado_feature_expression_hide_biomaterial_labels',TRUE),
  );

  $form['feature_expression']['limit_label_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum Label Length'),
    '#description' => t('Limit the length of biomaterial labels. Set to 0 for unlimited length.'),
    '#default_value' => variable_get('chado_feature_expression_limit_label_length','0'),
    '#size' => 5,
  );

  $form['feature_expression']['min_num_biomaterials'] = array(
    '#type' => 'textfield',
    '#title' => t('Required Number of Expression Values'),
    '#description' => t('Only show expression data for features with this number or greater of expression values.'),
    '#default_value' => variable_get('chado_feature_expression_min_num_biomaterials','0'),
    '#size' => 5,
  );

  $form['feature_expression']['biomaterial_display_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Expression Column Width'),
    '#description' => t('The width of each expression columns. If "One Dimensional Heatmap" is selected, this value will also be the height of the heatmap tile.'),
    '#default_value' => variable_get('chado_feature_expression_biomaterial_display_width','15'),
    '#size' => 5,
  );

  $form['feature_expression']['expression_display'] = array(
    '#type' => 'radios',
    '#title' => t('Default Heatmap Display'),
    '#options' => array(
      'tile' => t('One Dimensional Heatmap'),
      'column' => t('Chart Display'),
    ),
    '#default_value' => variable_get('chado_feature_expression_expression_display','column'),
  );

  return system_settings_form($form);
}

/**
 * Implements a function that validates and submits the form that configures the display of analysis: expression data.
 * 
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_config_admin_validate($form, &$form_state) {

  variable_set('chado_feature_expression_hide_expression', $form_state['values']['hide_expression']);
  variable_set('chado_feature_expression_hide_biomaterial_labels', $form_state['values']['hide_biomaterial_labels']);
  variable_set('chado_feature_expression_expression_display', $form_state['values']['expression_display']);
  variable_set('chado_feature_expression_min_num_biomaterials', $form_state['values']['min_num_biomaterials']);

  if ( $form_state['values']['limit_label_length'] < 0 ) {
    form_set_error('limit_label_length', t("Please provide a number 0 or greater for the maximum label length."));
  }
  else {
    variable_set('chado_feature_expression_limit_label_length', $form_state['values']['limit_label_length']);
  }

  if ( $form_state['values']['min_num_biomaterials'] < 0 ) {
    form_set_error('min_num_biomaterials', t("Please provide a number 0 or greater for the required number of expression values."));
  }
  else {
    variable_set('chado_feature_expression_min_num_biomaterials', $form_state['values']['min_num_biomaterials']);
  }

  if ( $form_state['values']['biomaterial_display_width'] < 1) { 
    form_set_error('biomaterial_display_width', t("Please provide a number 1 or greater for the column width."));
  }
  else {
    variable_set('chado_feature_expression_biomaterial_display_width', $form_state['values']['biomaterial_display_width']);
  }

}

