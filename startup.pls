(begin
(define make-point (lambda (x y) (begin 
                                    (define a (list x y))
                                    (define b (set-property "object-name" "point" a))
                                    (define b (set-property "size" 0 b)))))

(define make-line (lambda (p1 p2) (begin 
                                    (define a (list p1 p2))
                                    (define b (set-property "object-name" "line" a))
                                    (define b (set-property "thickness" 1 b)))))

(define origin (make-point 0 0))

(define make-text (lambda (str) (begin 
                                    (define a str)
                                    (define b (set-property "object-name" "text" a))
                                    (define b (set-property "position" origin b)))))

)