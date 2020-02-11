<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__expression_features_formatter extends ChadoFieldFormatter
{

  // The default label for this field.
  public static $default_label = 'Expression Features';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__expression_features');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/

  public function settingsForm($view_mode, $form, &$form_state)
  {

  }

  /**
   * @see ChadoFieldFormatter::View()
   *
   **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display)
  {
    $rows = $items[0]['value'];
    $header = $items[0]['header'];
    $pager = $items[0]['pager'];
    $pager = $this->ajaxifyPager($pager, $entity);

    $table = theme('table',
      [
        "header" => $header,
        "rows" => $rows,
      ]);


    $element[0] =
      [
        "#type" => "markup",
        "#markup" => $table . $pager,
      ];
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode)
  {
    return '';
  }

}
