(define make-point (lambda (x y) (begin 
                                    (define a (list x y))
                                    (define b (set-property "object-name" "point" a))
                                    (define b (set-property "size" 0 b)))))