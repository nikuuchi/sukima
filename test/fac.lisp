(defun f (n) if (< n 2) (+ 1 0) (* (f (- n 1))))
(print (f 3))
