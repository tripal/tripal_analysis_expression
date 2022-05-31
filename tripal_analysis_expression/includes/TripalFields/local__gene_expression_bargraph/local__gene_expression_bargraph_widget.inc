<?php
/**
 * @class
 * Purpose:
 *
 * Allowing edit?
 * Data:
 * Assumptions:
 */
class local__gene_expression_bargraph_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Expression Bargraph';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__gene_expression_bargraph');

  /**
   * Provides the form for editing of this field.
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
  }

  /**
   * Performs validation of the widgetForm.
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
  }

  /**
   * Performs extra commands when the entity form is submitted.
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
  }

}