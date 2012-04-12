(defun f (n) 
	(if (< n 2) (+ 0 1) (* n  (f (- n 1))))
)
(print (f 5))
