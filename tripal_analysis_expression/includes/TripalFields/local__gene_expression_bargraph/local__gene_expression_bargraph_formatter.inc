<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__gene_expression_bargraph_formatter extends ChadoFieldFormatter{

  // The default label for this field.
  public static $default_label = 'Expression Bargraph';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__gene_expression_bargraph'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * Provides the field's setting form.
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *  Provides the display for a field 
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if (count($items) == 0) {
      return;
    }
    
    $record = $entity->chado_record;
    $feature_id = $record->feature_id;
    
    // Load the necessary JS and CSS files.
    $js_path = url(drupal_get_path('module', 'tripal_analysis_expression') . '/theme/js', array('absolute' => TRUE));
    $css_path = url(drupal_get_path('module', 'tripal_analysis_expression') . '/theme/css', array('absolute' => TRUE));
    drupal_add_js('https://cdnjs.cloudflare.com/ajax/libs/d3/3.5.17/d3.min.js');
    drupal_add_js($js_path . '/expression.js');
    drupal_add_css($css_path . '/expression.css');
    
    // Get the form for specifying the network to view.
    $viewer_form = drupal_get_form('tripal_analysis_expression_feature_viewer_form',
        $feature_id);
    $content = drupal_render($viewer_form);
    $content .= '<figure id="analysis-expression-figure"></figure>';
    
    // Return the page.
    $element[0] =  [
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }

  /**
   * Provides a summary of the formatter settings.
   */
  public function settingsSummary($view_mode) {
    return '';
  }
}
