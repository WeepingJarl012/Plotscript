(begin
(define make-point (lambda (x y) (begin 
                                    (define hopeFullyNotUsed1 (list x y))
                                    (define hopeFullyNotUsed2 (set-property "object-name" "point" hopeFullyNotUsed1))
                                    (define hopeFullyNotUsed2 (set-property "size" 0 hopeFullyNotUsed2)))))

(define make-line (lambda (p1 p2) (begin 
                                    (define hopeFullyNotUsed3 (list p1 p2))
                                    (define hopeFullyNotUsed4 (set-property "object-name" "line" hopeFullyNotUsed3))
                                    (define hopeFullyNotUsed4 (set-property "thickness" 1 hopeFullyNotUsed4)))))

(define origin (make-point 0 0))

(define make-text (lambda (str) (begin 
                                    (define hopeFullyNotUsed5 str)
                                    (define hopeFullyNotUsed6 (set-property "object-name" "text" hopeFullyNotUsed5))
                                    (define hopeFullyNotUsed6 (set-property "position" origin hopeFullyNotUsed6)))))

)