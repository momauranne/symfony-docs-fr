invalid_message
~~~~~~~~~~~~~~~

**type**: ``string`` **default**: ``This value is not valid``

C'est le message d'erreur de validation qui est utilis� quand les donn�es
saisies sont valid�es par le type de champ. Cela arrive, par exemple, quand
l'utilisateur saisie une chaine de caract�res dans un champ :doc:`time</reference/forms/types/time>`
et que cette chaine ne peut �tre convertie en temps. Pour les messages de validation
classiques (par exemple quand vous d�finissez une longueur minimum dans un champ), d�finissez
les avec vos r�gles de validation (:ref:`reference<book-validation-constraint-configuration>`).